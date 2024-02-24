const project = new Project('MultiWindow');

await project.addProject(findKinc());

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
