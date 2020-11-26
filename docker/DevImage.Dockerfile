FROM esydev/esy:nightly-alpine-latest

# perl-utils for shasum
RUN apk add nodejs npm linux-headers emacs curl git perl-utils
COPY . /app
WORKDIR /app
RUN esy --cache-tarballs-path=esy-sources
RUN esy release
WORKDIR /app/_release
RUN npm pack
