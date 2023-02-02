const project = new Project('Example');

await project.addProject('../Kinc');

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
