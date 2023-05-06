#!/usr/bin/env bash

curl -s https://raw.githubusercontent.com/k3d-io/k3d/main/install.sh | bash
sudo dnf install helm
k3d cluster create local

