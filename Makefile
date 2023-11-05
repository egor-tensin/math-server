include prelude.mk

this_dir    := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
src_dir     := $(this_dir)
build_dir   := $(this_dir)build
boost_dir   := $(build_dir)/boost
cmake_dir   := $(build_dir)/cmake
install_dir := $(build_dir)/install

PROJECT         := math-server
TOOLSET         ?= auto
PLATFORM        ?= auto
CONFIGURATION   ?= Debug
BOOST_VERSION   ?= 1.72.0
BOOST_LIBRARIES := --with-filesystem --with-program_options --with-regex --with-test
CMAKE_FLAGS     ?= -D MATH_SERVER_TESTS=ON
INSTALL_PREFIX  ?= $(install_dir)

# Target platforms (used by buildx):
DOCKER_PLATFORMS := amd64,armhf,arm64
# Docker Hub credentials:
DOCKER_USERNAME  := egortensin

$(eval $(call noexpand,TOOLSET))
$(eval $(call noexpand,PLATFORM))
$(eval $(call noexpand,CONFIGURATION))
$(eval $(call noexpand,BOOST_VERSION))
$(eval $(call noexpand,CMAKE_FLAGS))
ifdef DOCKER_PASSWORD
$(eval $(call noexpand,DOCKER_PASSWORD))
endif
$(eval $(call noexpand,INSTALL_PREFIX))

.PHONY: DO
DO:

.PHONY: all
all: build

.PHONY: clean
clean:
	rm -rf -- '$(call escape,$(build_dir))'

$(boost_dir)/:
	cd cmake && python3 -m project.boost.download \
		--cache '$(call escape,$(build_dir))' \
		-- \
		'$(call escape,$(BOOST_VERSION))' \
		'$(call escape,$(boost_dir))'

.PHONY: deps
deps: $(boost_dir)/
	cd cmake && python3 -m project.boost.build \
		--toolset '$(call escape,$(TOOLSET))' \
		--platform '$(call escape,$(PLATFORM))' \
		--configuration '$(call escape,$(CONFIGURATION))' \
		-- \
		'$(call escape,$(boost_dir))' \
		$(BOOST_LIBRARIES)

.PHONY: build
build:
	cd cmake && python3 -m project.build \
		--toolset '$(call escape,$(TOOLSET))' \
		--platform '$(call escape,$(PLATFORM))' \
		--configuration '$(call escape,$(CONFIGURATION))' \
		--install '$(call escape,$(INSTALL_PREFIX))' \
		--boost '$(call escape,$(boost_dir))' \
		-- \
		'$(call escape,$(src_dir))' \
		'$(call escape,$(cmake_dir))' \
		$(CMAKE_FLAGS)

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
	@echo '$(call escape,$(DOCKER_PASSWORD))' \
		| docker login --username '$(call escape,$(DOCKER_USERNAME))' --password-stdin

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
	docker-compose build --progress plain

.PHONY: compose/push
# `docker-compose push` has the same problems as `docker push`.
compose/push: docker/check-push compose/build
	docker-compose push

.PHONY: buildx/create
buildx/create:
	docker buildx create --use --name '$(call escape,$(PROJECT))_builder'

.PHONY: buildx/rm
buildx/rm:
	docker buildx rm '$(call escape,$(PROJECT))_builder'

buildx/build/%: DO
	docker buildx build \
		-f '$*/Dockerfile' \
		-t '$(call escape,$(DOCKER_USERNAME))/math-$*' \
		--platform '$(call escape,$(DOCKER_PLATFORMS))' \
		--progress plain \
		.

.PHONY: buildx/build
buildx/build: buildx/build/client buildx/build/server

buildx/push/%: DO
	docker buildx build \
		-f '$*/Dockerfile' \
		-t '$(call escape,$(DOCKER_USERNAME))/math-$*' \
		--platform '$(call escape,$(DOCKER_PLATFORMS))' \
		--progress plain \
		--push \
		.

.PHONY: buildx/push
buildx/push: buildx/push/client buildx/push/server
