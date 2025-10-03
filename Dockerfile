FROM ubuntu:20.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    curl \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    libxml2-dev \
    libxmlsec1-dev \
    libffi-dev \
    liblzma-dev \
    wget \
    git \
    tzdata \
    gcc \
    g++ \
    python \
    python-dev \
    python-pygccxml \
    mercurial \
    valgrind \
    flex \
    bison \
    libfl-dev \
    tcpdump \
    vtun \
    lxc \
    uncrustify \
    python-pygraphviz \
    python-kiwi \
    python-pygoocanvas \
    gcc-6 g++-6 \    #追加
    libgoocanvas-dev

# Build OpenSSL manually
WORKDIR /opt
RUN wget https://www.openssl.org/source/openssl-1.1.1u.tar.gz && \
    tar xzf openssl-1.1.1u.tar.gz && \
    cd openssl-1.1.1u && \
    ./config --prefix=/opt/openssl --openssldir=/opt/openssl && \
    make -j$(nproc) && \
    make install

# Build Python 3.10.13 with custom OpenSSL
WORKDIR /opt
RUN wget https://www.python.org/ftp/python/3.10.13/Python-3.10.13.tgz && \
    tar xzf Python-3.10.13.tgz && \
    cd Python-3.10.13 && \
    env CPPFLAGS="-I/opt/openssl/include" \
        LDFLAGS="-L/opt/openssl/lib" \
        LD_LIBRARY_PATH="/opt/openssl/lib" \
        ./configure --enable-optimizations --with-openssl=/opt/openssl && \
    make -j$(nproc) && \
    make altinstall

# Install pip and pybind11
RUN /usr/local/bin/python3.10 -m ensurepip && \
    /usr/local/bin/python3.10 -m pip install --upgrade pip && \
    /usr/local/bin/python3.10 -m pip install pybind11

# Build ns-3/mptcp
WORKDIR /src
RUN git clone https://github.com/mkheirkhah/mptcp.git && \
    cd mptcp && \
    ./waf -d debug --enable-examples --enable-tests configure --cxx=/usr/bin/g++-6　&& \
    ./waf

# Clean up
RUN apt-get clean