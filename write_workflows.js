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
  const steps = workflow.steps ?? '';
  const postfixSteps = workflow.postfixSteps ?? '';

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

    runs-on: ${workflow.runsOn}

    steps:
    - uses: actions/checkout@v2
${steps}
    - name: Get Submodules
      run: ./get_dlc
${postfixSteps}
`;

  for (const sample of samples) {
    if (sample === 'RuntimeShaderCompilation') {
      if (!workflow.RuntimeShaderCompilation) {
        continue;
      }
    }

    if (workflow.noCompute && sample === 'ComputeShader') {
      continue;
    }

    if (workflow.noTexArray && sample === 'TextureArray') {
      continue;
    }

    const prefix = workflow.compilePrefix ?? '';
    const postfix = workflow.compilePostfix ?? '';
    const gfx = (workflow.gfx === 'WebGL') ? 'opengl' : workflow.gfx.toLowerCase();
    const options = workflow.options ? ' ' + workflow.options : '';
    const sys = workflow.sys === 'macOS' ? 'osx' : workflow.sys.toLowerCase();

    workflowText +=
`    - name: Compile ${sample}
      working-directory: ${sample}
      run: ${prefix}../Kinc/make ${sys} -g ${gfx}${options} --compile${postfix}
`;
  }

  fs.writeFileSync(path.join(workflowsDir, workflow.sys.toLowerCase() + '-' + workflow.gfx.toLowerCase() + '.yml'), workflowText, {encoding: 'utf8'});
}

const workflows = [
  {
    sys: 'Android',
    gfx: 'OpenGL',
    runsOn: 'ubuntu-latest'
  },
  {
    sys: 'Android',
    gfx: 'Vulkan',
    runsOn: 'ubuntu-latest'
  },
  {
    sys: 'Emscripten',
    gfx: 'WebGL',
    runsOn: 'ubuntu-latest',
    steps: '',
    compilePrefix: '../emsdk/emsdk activate latest && source ../emsdk/emsdk_env.sh && ',
    compilePostfix: ' && cd build/Release && make',
    postfixSteps:
`    - name: Setup emscripten
      run: git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest
`,
    noCompute: true,
    noTexArray: true
  },
  {
    sys: 'Emscripten',
    gfx: 'WebGPU',
    runsOn: 'ubuntu-latest',
    steps: '',
    compilePrefix: '../emsdk/emsdk activate latest && source ../emsdk/emsdk_env.sh && ',
    compilePostfix: ' && cd build/Release && make',
    postfixSteps:
`    - name: Setup emscripten
      run: git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest
`
  },
  {
    sys: 'iOS',
    gfx: 'Metal',
    runsOn: 'macOS-latest',
    options: '--nosigning'
  },
  {
    sys: 'iOS',
    gfx: 'OpenGL',
    runsOn: 'macOS-latest',
    options: '--nosigning',
    noCompute: true
  },
  {
    sys: 'Linux',
    gfx: 'OpenGL',
    runsOn: 'ubuntu-latest',
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
    runsOn: 'ubuntu-latest',
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
  },
  {
    sys: 'macOS',
    gfx: 'Metal',
    runsOn: 'macOS-latest',
    RuntimeShaderCompilation: true
  },
  {
    sys: 'macOS',
    gfx: 'OpenGL',
    runsOn: 'macOS-latest'
  }
];

for (const workflow of workflows) {
  writeWorkflow(workflow);
}
