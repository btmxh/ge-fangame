{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/default";
    flake-utils.url = "github:numtide/flake-utils";
    git-hooks.url = "github:cachix/git-hooks.nix";
  };

  outputs =
    {
      self,
      nixpkgs,
      systems,
      git-hooks,
      ...
    }:
    let
      forEachSystem = nixpkgs.lib.genAttrs (import systems);
    in
    {
      devShells = forEachSystem (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
          python = pkgs.python3.withPackages (ps: with ps; [ pillow ]);
          inherit (self.checks.${system}.pre-commit-check) shellHook enabledPackages;
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              cmake
              ninja
              pkg-config

              # this order matters: we want to have the host gcc as the default option
              gcc-arm-embedded
              gcc

              python
              # python3.withPackages (ps: with ps; [ numpy ])
            ];

            buildInputs =
              with pkgs;
              [
                sdl3
              ]
              ++ enabledPackages;

            shellHook = shellHook + ''
              export CMAKE_EXPORT_COMPILE_COMMANDS=1
              export CMAKE_GENERATOR="Ninja Multi-Config"
            '';

            packages = with pkgs; [
              openocd
              gdb

              # tooling
              clang-tools
              nixd
              nixfmt-rfc-style

              # cmake
              gersemi
              cmake-lint
              neocmakelsp
            ];
          };
        }
      );

      checks = forEachSystem (system: {
        pre-commit-check = git-hooks.lib.${system}.run {
          src = ./.;
          hooks = {
            nixfmt.enable = true;
            statix.enable = true;
            check-yaml.enable = true;
            end-of-file-fixer.enable = true;
            trim-trailing-whitespace.enable = true;
            ruff.enable = true;
            ruff-format.enable = true;
          };
        };
      });
    };
}
