# Various one-liners which I'm too lazy to remember.
# Basically a collection of really small shell scripts.

PROJECT = math_server
# Enable buildx support:
export DOCKER_CLI_EXPERIMENTAL = enabled
# Target platforms (used by buildx):
platforms = linux/amd64,linux/armhf
# Docker Hub credentials:
DOCKER_USERNAME = egortensin

all: build

login:
ifndef DOCKER_PASSWORD
	$(error Please define DOCKER_PASSWORD)
endif
	@echo "$(DOCKER_PASSWORD)" | docker login --username "$(DOCKER_USERNAME)" --password-stdin

# Re-register binfmt_misc formats with the F flag (required i.e. on Bionic):
fix-binfmt:
	docker run --rm --privileged docker/binfmt:66f9012c56a8316f9244ffd7622d7c21c1f6f28d

binfmt: fix-binfmt

# `docker build` has week support for multiarch repos (you need to use multiple
# Dockerfile's, create a manifest manually, etc.), so it's only here for
# testing purposes, and native builds.
docker-build/%:
ifndef FORCE
	$(warning Consider using `docker buildx` instead)
endif
	docker build -f "$*/Dockerfile" -t "$(DOCKER_USERNAME)/math-$*" .

docker-build: docker-build/client docker-build/server

# `docker-compose build` has the same problems as `docker build`.
compose-build:
ifndef FORCE
	$(warning Consider using `docker buildx` instead)
endif
	docker-compose build

# The simple way to build multiarch repos.
builder/create: fix-binfmt
	docker buildx create --use --name "$(PROJECT)_builder"

builder/rm:
	docker buildx rm "$(PROJECT)_builder"

buildx/%:
	docker buildx build -f "$*/Dockerfile" -t "$(DOCKER_USERNAME)/math-$*" --platform "$(platforms)" --progress plain .

buildx: buildx/client buildx/server

# Build natively by default.
build: compose-build

# `docker push` would replace the multiarch repo with a single image by default
# (you'd have to create a manifest and push it instead), so it's only here for
# testing purposes.
check-docker-push:
ifndef FORCE
	$(error Please do not use `docker push`)
endif

docker-push/%: check-docker-push docker-build/%
	docker push "$(DOCKER_USERNAME)/math-$*"

docker-push: check-docker-push docker-push/client docker-push/server

# `docker-compose push` has the same problems as `docker push`.
check-compose-push:
ifndef FORCE
	$(error Please do not use `docker-compose push`)
endif

compose-push: check-compose-push compose-build
	docker-compose push

# The simple way to push multiarch repos.
buildx-push/%:
	docker buildx build -f "$*/Dockerfile" -t "$(DOCKER_USERNAME)/math-$*" --platform "$(platforms)" --progress plain --push .

buildx-push: buildx-push/client buildx-push/server

# buildx is used by default.
push: buildx-push

pull:
	docker-compose pull

up:
	docker-compose up -d server

run/client:
	docker-compose --rm run client

down:
	docker-compose down --volumes

clean:
	docker system prune --all --force --volumes

.PHONY: all login fix-binfmt binfmt docker-build compose-build builder/create builder/rm buildx build check-docker-push docker-push check-compose-push compose-push buildx-push push pull up run/client down clean
