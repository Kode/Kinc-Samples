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

  const workflowName = workflow.gfx ? (workflow.sys + ' (' + workflow.gfx + ')') : workflow.sys;
  let workflowText = `name: ${workflowName}

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
    const gfx = workflow.gfx ? ((workflow.gfx === 'WebGL') ? ' -g opengl' : ' -g ' + workflow.gfx.toLowerCase().replace(/ /g, '')) : '';
    const options = workflow.options ? ' ' + workflow.options : '';
    const sys = workflow.sys === 'macOS' ? 'osx' : (workflow.sys === 'UWP' ? 'windowsapp' : workflow.sys.toLowerCase());
    const vs = workflow.vs ? ' -v ' + workflow.vs : '';

    workflowText +=
`    - name: Compile ${sample}
      working-directory: ${sample}
      run: ${prefix}../Kinc/make ${sys}${vs}${gfx}${options} --compile${postfix}
`;
    if (workflow.env) {
      workflowText += workflow.env;
    }
  }

  const name = workflow.gfx ? (workflow.sys.toLowerCase() + '-' + workflow.gfx.toLowerCase().replace(/ /g, '')) : workflow.sys.toLowerCase();
  fs.writeFileSync(path.join(workflowsDir, name + '.yml'), workflowText, {encoding: 'utf8'});
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
  },
  {
    sys: 'UWP',
    runsOn: 'windows-latest',
    vs: 'vs2022'
  },
  {
    sys: 'Windows',
    gfx: 'Direct3D 9',
    runsOn: 'windows-latest',
    noCompute: true,
    noTexArray: true,
    vs: 'vs2022',
    postfixSteps:
`    - name: Install DirectX
      run: choco install -y directx --no-progress`
  },
  {
    sys: 'Windows',
    gfx: 'Direct3D 11',
    runsOn: 'windows-latest',
    RuntimeShaderCompilation: true,
    vs: 'vs2022'
  },
  {
    sys: 'Windows',
    gfx: 'Direct3D 12',
    runsOn: 'windows-latest',
    vs: 'vs2022'
  },
  {
    sys: 'Windows',
    gfx: 'OpenGL',
    runsOn: 'windows-latest',
    vs: 'vs2022'
  },
  {
    sys: 'Windows',
    gfx: 'Vulkan',
    runsOn: 'windows-latest',
    vs: 'vs2022',
    env:
`      env:
        VULKAN_SDK: C:\\VulkanSDK\\1.2.189.2
`,
    steps:
`    - name: Setup Vulkan
      run: |
          Invoke-WebRequest -Uri "https://sdk.lunarg.com/sdk/download/1.2.189.2/windows/VulkanSDK-1.2.189.2-Installer.exe" -OutFile VulkanSDK.exe
          $installer = Start-Process -FilePath VulkanSDK.exe -Wait -PassThru -ArgumentList @("--da", "--al", "-c", "in");
          $installer.WaitForExit();`
  }
];

for (const workflow of workflows) {
  writeWorkflow(workflow);
}
