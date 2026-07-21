{ pkgs, modulesPath, ...}: {
  imports = [ (modulesPath + "/installer/cd-dvd/installation-cd-minimal.nix") ];

  services.displayManager.cosmic-greeter.enable = true;
  services.desktopManager.cosmic.enable = true;
  services.displayManager.autoLogin = {
    enable = true;
    user = "nixos";
  };
}