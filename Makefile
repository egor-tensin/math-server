# Various one-liners which I'm too lazy to remember.
# Basically a collection of really small shell scripts.

MAKEFLAGS += --warn-undefined-variables
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.SUFFIXES:
SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c

.PHONY: DO
DO:

PROJECT := math-server
# Enable buildx support:
export DOCKER_CLI_EXPERIMENTAL := enabled
# Target platforms (used by buildx):
PLATFORMS := linux/amd64,linux/armhf
# In case buildx isn't installed (e.g. on Ubuntu):
BUILDX_VERSION := v0.4.2
# Docker Hub credentials:
DOCKER_USERNAME := egortensin

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

ifdef DOCKER_PASSWORD
$(eval $(call noexpand,DOCKER_PASSWORD))
endif

curl := curl --silent --show-error --location --dump-header - --connect-timeout 20

.PHONY: all
all: docker/build

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

.PHONY: fix-binfmt
# Re-register binfmt_misc formats with the F flag (required e.g. on Bionic):
fix-binfmt:
	docker run --rm --privileged docker/binfmt:66f9012c56a8316f9244ffd7622d7c21c1f6f28d

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
	docker buildx build -f '$*/Dockerfile' -t '$(call escape,$(DOCKER_USERNAME))/math-$*' --platform '$(call escape,$(PLATFORMS))' --progress plain .

.PHONY: buildx/build
buildx/build: buildx/build/client buildx/build/server

buildx/push/%: DO
	docker buildx build -f '$*/Dockerfile' -t '$(call escape,$(DOCKER_USERNAME))/math-$*' --platform '$(call escape,$(PLATFORMS))' --progress plain --push .

.PHONY: buildx/push
buildx/push: buildx/push/client buildx/push/server
