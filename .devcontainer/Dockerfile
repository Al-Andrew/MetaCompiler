FROM ubuntu:22.04


RUN export DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq
RUN apt-get install -y --no-install-recommends software-properties-common wget apt-utils file zip openssh-client gpg-agent socat rsync make ninja-build git cmake clang flex bison clangd clang-format gdb lldb valgrind cppcheck clang-tidy