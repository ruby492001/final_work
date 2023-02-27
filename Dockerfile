#building qt
FROM ubuntu:20.04 AS build_env
ARG DEBIAN_FRONTEND=noninteractive
RUN echo "deb http://security.ubuntu.com/ubuntu focal-security main" >>  /etc/apt/sources.list.d/focal-security.list
RUN apt-get update &&  \
    apt-get install -y  make  \
                        apt-utils\
                        cmake \
                        g++ \
                        gcc \
                        automake \
                        build-essential \
                        libssl-dev \
                        libssl1.1 \
                        gdb \
                        clang

RUN apt install -y bison build-essential clang flex gperf \
    libatspi2.0-dev libbluetooth-dev libclang-dev libcups2-dev libdrm-dev \
    libegl1-mesa-dev libfontconfig1-dev libfreetype6-dev \
    libgstreamer1.0-dev libhunspell-dev libnss3-dev libopengl-dev \
    libpulse-dev libssl-dev libts-dev libx11-dev libx11-xcb-dev \
    libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev \
    libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev \
    libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev \
    libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev libxcb1-dev \
    libxcomposite-dev libxcursor-dev libxdamage-dev libxext-dev \
    libxfixes-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev \
    libxkbfile-dev libxrandr-dev libxrender-dev libxshmfence-dev \
    libxshmfence1 llvm ninja-build nodejs python-is-python3 python3


FROM build_env AS log_collector_qt
COPY qt6 /usr/src/qt6
WORKDIR /usr/src/qt6
RUN mkdir -p qt6-build &&  \
    cd qt6-build &&  \
    ../configure -prefix /opt/qt6 &&  \
    cmake --build . --parallel 8 &&  \
    cmake --install .



#building app
#FROM log_collector_qt as log_collector_building
#RUN mkdir -p /usr/src/log_collector
#COPY src /usr/src/log_collector
#WORKDIR /usr/src/log_collector
#RUN mkdir -p build && cd build && ls -la ../ && cmake ../ && make


#FROM scratch AS log_collector_packages_export
#COPY --from=log_collector_building /usr/src/log_collector/build/log_collector/testbin1/testbin1 /

