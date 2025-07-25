# Dockerfile
FROM debian:bullseye

RUN apt update && apt install -y \
	g++ \
	make \
	libncurses-dev \
	dpkg-dev \
	fakeroot \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /navix

COPY . .

CMD ["bash"]
