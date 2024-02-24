const project = new Project('PlayVideo');

await project.addProject(findKinc());

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
