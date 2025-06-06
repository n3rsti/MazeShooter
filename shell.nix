{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  name = "opengl-dev-shell";

  buildInputs =
    [ pkgs.glew pkgs.glfw pkgs.glm pkgs.pkg-config pkgs.gcc pkgs.bear pkgs.assimp ];

  shellHook = ''
    export PKG_CONFIG_PATH=${pkgs.glew}/lib/pkgconfig:${pkgs.glfw}/lib/pkgconfig:${pkgs.assimp}/lib/pkgconfig:$PKG_CONFIG_PATH
  '';
}
