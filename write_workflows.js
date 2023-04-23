const fs = require('fs');
const path = require('path');

const samples = [
	'Shader',
	'Texture',
	'MultiWindow',
	'ComputeShader',
	'TextureArray',
	'ShaderG5',
  'RuntimeShaderCompilation'
];

const workflowsDir = path.join('.github', 'workflows');

function writeWorkflow(workflow) {
  let workflowText = `name: ${workflow.sys} (${workflow.gfx})

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
${workflow.steps}
    - name: Get Submodules
      run: ./get_dlc
`;

  for (const sample of samples) {
    if (sample === 'RuntimeShaderCompilation') {
      if (!workflow.RuntimeShaderCompilation) {
        continue;
      }
    }
    workflowText +=
`    - name: Compile ${sample}
      working-directory: ${sample}
      run: ../Kinc/make ${workflow.sys.toLowerCase()} -g ${workflow.gfx.toLowerCase()} --compile
`;
  }

  fs.writeFileSync(path.join(workflowsDir, workflow.sys.toLowerCase() + '-' + workflow.gfx.toLowerCase() + '.yml'), workflowText, {encoding: 'utf8'});
}

const workflows = [
  {
    sys: 'Android',
    gfx: 'OpenGL',
    steps: ''
  },
  {
    sys: 'Android',
    gfx: 'Vulkan',
    steps: ''
  },
  {
    sys: 'Linux',
    gfx: 'OpenGL',
    steps:
`    - name: Apt Update
      run: sudo apt update
    - name: Apt Install
      run: sudo apt-get install libasound2-dev libxinerama-dev libxrandr-dev libgl1-mesa-dev libxi-dev libxcursor-dev libudev-dev libwayland-dev wayland-protocols libxkbcommon-dev ninja-build --yes --quiet
`,
    RuntimeShaderCompilation: true
  },
  {
    sys: 'Linux',
    gfx: 'Vulkan',
    steps:
`    - name: Get LunarG key
      run: wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
    - name: Get LunarG apt sources
      run: sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
    - name: Apt Update
      run: sudo apt update
    - name: Apt Install
      run: sudo apt install libasound2-dev libxinerama-dev libxrandr-dev libgl1-mesa-dev libxi-dev libxcursor-dev libudev-dev vulkan-sdk libwayland-dev wayland-protocols libxkbcommon-dev ninja-build --yes --quiet
`
  }
];

for (const workflow of workflows) {
  writeWorkflow(workflow);
}
