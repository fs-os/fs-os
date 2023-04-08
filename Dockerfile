FROM 8dcc/i386-cross-compiler
COPY . /fs-os
WORKDIR /fs-os

# Install dependencies for building the package
RUN apt-get update && \
    apt-get install -y sudo xorriso qemu-system-i386

RUN make

CMD [ "make", "qemu" ]
