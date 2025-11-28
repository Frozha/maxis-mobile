FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    git \
    python3 \
    python3-pip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY /.devcontainer/requirements.txt /workspace/

RUN pip install --upgrade pip &&\
    pip install -r requirements.txt 



EXPOSE 8888

CMD ["bash"]
