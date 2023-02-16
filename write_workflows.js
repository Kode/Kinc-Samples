const fs = require('fs');
const path = require('path');

const samples = [
	'Shader',
	'Texture',
	'MultiWindow',
	'ComputeShader',
	'TextureArray',
	'ShaderG5'
];

const workflowsDir = path.join('.github', 'workflows');

let workflow =
`name: Linux (Vulkan)

on:
  push:
    branches:
    - main
  pull_request:
    branches:
    - main

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2
    - name: Get LunarG key
      run: wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
    - name: Get LunarG apt sources
      run: sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
    - name: Apt Update
      run: sudo apt update
    - name: Apt Install
      run: sudo apt install libasound2-dev libxinerama-dev libxrandr-dev libgl1-mesa-dev libxi-dev libxcursor-dev libudev-dev vulkan-sdk libwayland-dev wayland-protocols libxkbcommon-dev ninja-build --yes --quiet
    - name: Get Submodules
      run: ./get_dlc
`;

for (const sample of samples) {
	workflow +=
`    - name: Compile ${sample}
      working-directory: ${sample}
      run: ../Kinc/make -g vulkan --compile
`;
}

fs.writeFileSync(path.join(workflowsDir, 'linux-vulkan.yml'), workflow, {encoding: 'utf8'});
