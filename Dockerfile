FROM ubuntu:20.04

# タイムゾーン設定を非対話モードにする（重要！）
ENV DEBIAN_FRONTEND=noninteractive
RUN ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime

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
    python2 \
    python-is-python2 \
    mercurial \
    valgrind \
    flex \
    bison \
    libfl-dev \
    tcpdump \
    vtun \
    lxc \
    uncrustify \
    python3-pygraphviz \
    gcc-7 g++-7 \
    libgoocanvas-2.0-dev

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
    /usr/local/bin/python3.10 -m pip install pybind11 pygccxml

ENV CXXFLAGS="-O3 -g -Wall -std=c++17 -Wno-error=deprecated-declarations -Wno-error=format-overflow -Wno-error=nonnull-compare"

# Build ns-3/mptcp
WORKDIR /src
RUN git clone https://github.com/mkheirkhah/mptcp.git && \
    cd mptcp && \
    rm -rf build || true && \
    sed -i '/Module.*wimax/s/^/#/' wscript && \ 
    find . -name 'wscript' -exec sed -i 's/-Werror//g' {} + && \
    find . -name 'wscript' -exec sed -i 's/-std=c++11//g' {} + && \

    find examples scratch -name "*.cc" -exec sed -i '/payload_buffer/! s/uint8_t data\[writeSize\]/uint8_t payload_buffer\[writeSize\]/g; /payload_buffer/! s/uint8_t buffer\[writeSize\]/uint8_t payload_buffer\[writeSize\]/g' {} + && \
    find examples scratch -name "*.cc" -exec sed -i '/payload_buffer/! s/\&data\[dataOffset\]/\&payload_buffer\[dataOffset\]/g; /payload_buffer/! s/\&buffer\[dataOffset\]/\&payload_buffer\[dataOffset\]/g' {} + && \
    find examples scratch -name "*.cc" -exec sed -i '/payload_buffer/! s/data\[i\] = m;/payload_buffer\[i\] = m;/g; /payload_buffer/! s/buffer\[i\] = m;/payload_buffer\[i\] = m;/g' {} + && \

    python2 ./waf -d optimized --enable-examples --enable-tests configure && \
    python2 ./waf

# Clean up
RUN apt-get clean