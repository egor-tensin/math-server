# Various one-liners which I'm too lazy to remember.
# Basically a collection of really small shell scripts.

MAKEFLAGS += --warn-undefined-variables
unexport MAKEFLAGS
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.SUFFIXES:
SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c

.PHONY: DO
DO:

escape = $(subst ','\'',$(1))

define noexpand
ifeq ($$(origin $(1)),environment)
    $(1) := $$(value $(1))
endif
ifeq ($$(origin $(1)),environment override)
    $(1) := $$(value $(1))
endif
ifeq ($$(origin $(1)),command line)
    override $(1) := $$(value $(1))
endif
endef

PROJECT := math-server
TOOLSET ?= auto
CONFIGURATION ?= Debug
BOOST_VERSION ?= 1.72.0
BOOST_LIBRARIES := --with-filesystem --with-program_options --with-regex --with-test
CMAKE_FLAGS ?=

this_dir  := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
src_dir   := $(this_dir)
ifdef CI
build_dir := $(this_dir)../build
else
build_dir := $(this_dir).build
endif
boost_dir := $(build_dir)/boost
cmake_dir := $(build_dir)/cmake
DESTDIR   ?= $(build_dir)/install

# Enable buildx support:
export DOCKER_CLI_EXPERIMENTAL := enabled
# Target platforms (used by buildx):
DOCKER_PLATFORMS := linux/amd64,linux/armhf
# In case buildx isn't installed (e.g. on Ubuntu):
BUILDX_VERSION := v0.4.2
# Docker Hub credentials:
DOCKER_USERNAME := egortensin

$(eval $(call noexpand,TOOLSET))
$(eval $(call noexpand,CONFIGURATION))
$(eval $(call noexpand,BOOST_VERSION))
$(eval $(call noexpand,CMAKE_FLAGS))
ifdef DOCKER_PASSWORD
$(eval $(call noexpand,DOCKER_PASSWORD))
endif
$(eval $(call noexpand,DESTDIR))

.PHONY: all
all: build

.PHONY: clean
clean:
	rm -rf -- '$(call escape,$(build_dir))'

$(boost_dir)/:
	cd cmake && python3 -m project.boost.download --cache '$(call escape,$(build_dir))' -- '$(call escape,$(BOOST_VERSION))' '$(call escape,$(boost_dir))'

.PHONY: deps
ifdef CI
deps:
	cd cmake && python3 -m project.ci.boost -- $(BOOST_LIBRARIES)
else
deps: $(boost_dir)/
	cd cmake && python3 -m project.boost.build --toolset '$(call escape,$(TOOLSET))' --configuration '$(call escape,$(CONFIGURATION))' -- '$(call escape,$(boost_dir))' $(BOOST_LIBRARIES)
endif

.PHONY: build
build:
ifdef CI
	cd cmake && python3 -m project.ci.cmake --install -- -D MATH_SERVER_TESTS=ON $(CMAKE_FLAGS)
else
	cd cmake && python3 -m project.cmake.build --toolset '$(call escape,$(TOOLSET))' --configuration '$(call escape,$(CONFIGURATION))' --build '$(call escape,$(cmake_dir))' --install '$(call escape,$(DESTDIR))' --boost '$(call escape,$(boost_dir))' -- '$(call escape,$(src_dir))' -D MATH_SERVER_TESTS=ON $(CMAKE_FLAGS)
endif

.PHONY: install
install: build

.PHONY: test
test:
	cd -- '$(call escape,$(cmake_dir))' && ctest -C '$(call escape,$(CONFIGURATION))' --verbose

.PHONY: docker/login
docker/login:
ifndef DOCKER_PASSWORD
	$(error Please define DOCKER_PASSWORD)
endif
	@echo '$(call escape,$(DOCKER_PASSWORD))' | docker login --username '$(call escape,$(DOCKER_USERNAME))' --password-stdin

.PHONY: docker/build
# Build using Compose by default.
docker/build: compose/build

.PHONY: docker/clean
docker/clean:
	docker system prune --all --force --volumes

.PHONY: docker/push
# Push multi-arch images by default.
docker/push: buildx/push

.PHONY: pull
pull:
	docker-compose pull

.PHONY: up
up:
	docker-compose up -d server

.PHONY: client
client:
	docker-compose run --rm client

.PHONY: down
down:
	docker-compose down --volumes

# `docker build` has weak support for multiarch repos (you need to use multiple
# Dockerfile's, create a manifest manually, etc.).

.PHONY: docker/check-build
docker/check-build:
ifndef FORCE
	$(warning Going to build natively; consider `docker buildx build` instead)
endif

# `docker push` would replace the multiarch repo with a single image by default
# (you'd have to create a manifest and push it instead).

.PHONY: docker/check-push
docker/check-push:
ifndef FORCE
	$(error Please use `docker buildx build --push` instead)
endif

.PHONY: compose/build
# `docker-compose build` has the same problems as `docker build`.
compose/build: docker/check-build
	docker-compose build

.PHONY: compose/push
# `docker-compose push` has the same problems as `docker push`.
compose/push: docker/check-push compose/build
	docker-compose push

# The simple way to build multiarch repos is `docker buildx`.

binfmt_image := docker/binfmt:66f9012c56a8316f9244ffd7622d7c21c1f6f28d

.PHONY: fix-binfmt
fix-binfmt:
	docker run --rm --privileged '$(call escape,$(binfmt_image))'

curl := curl --silent --show-error --location --dump-header - --connect-timeout 20

buildx_url := https://github.com/docker/buildx/releases/download/$(BUILDX_VERSION)/buildx-$(BUILDX_VERSION).linux-amd64

.PHONY: buildx/install
buildx/install:
	mkdir -p -- ~/.docker/cli-plugins/
	$(curl) --output ~/.docker/cli-plugins/docker-buildx -- '$(call escape,$(buildx_url))'
	chmod +x -- ~/.docker/cli-plugins/docker-buildx

.PHONY: buildx/create
buildx/create: fix-binfmt
	docker buildx create --use --name '$(call escape,$(PROJECT))_builder'

.PHONY: buildx/rm
buildx/rm:
	docker buildx rm '$(call escape,$(PROJECT))_builder'

buildx/build/%: DO
	docker buildx build -f '$*/Dockerfile' -t '$(call escape,$(DOCKER_USERNAME))/math-$*' --platform '$(call escape,$(DOCKER_PLATFORMS))' --progress plain .

.PHONY: buildx/build
buildx/build: buildx/build/client buildx/build/server

buildx/push/%: DO
	docker buildx build -f '$*/Dockerfile' -t '$(call escape,$(DOCKER_USERNAME))/math-$*' --platform '$(call escape,$(DOCKER_PLATFORMS))' --progress plain --push .

.PHONY: buildx/push
buildx/push: buildx/push/client buildx/push/server
