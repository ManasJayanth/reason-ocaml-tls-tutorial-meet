FROM esydev/esy:nightly-alpine-latest

# perl-utils for shasum
# bash because even esy-ocaml/ocaml uses it in ./esy-configure
RUN apk add nodejs npm linux-headers emacs curl git perl-utils bash
COPY . /app
WORKDIR /app
RUN esy --cache-tarballs-path=esy-sources
RUN esy release
WORKDIR /app/_release
RUN npm pack
