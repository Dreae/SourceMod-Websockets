FROM fedora:32

RUN dnf -y install glibc-static.i686 openssl-devel.i686 libstdc++-static.i686 glibc-devel.i686 cmake make gcc g++
RUN curl -o boost.tar.gz -L https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz
RUN tar -xvf boost.tar.gz
RUN cd boost_1_75_0 && ./bootstrap.sh && ./b2 --with-system variant=release link=static address-model=32