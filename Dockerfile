FROM 8dcc/i386-cross-compiler
COPY . /fs-os
WORKDIR /fs-os

# Install dependencies for building the package
RUN apt-get update && \
    apt-get install -y sudo git xorriso libgtk-3-dev qemu-system-i386

# Make sure submodules are cloned
RUN git submodule update --init --recursive && \
    git submodule foreach git checkout main

RUN make

CMD [ "make", "qemu" ]
