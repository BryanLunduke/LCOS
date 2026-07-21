{ lib, ... }: { 
  imports = [
    ./installConfig.nix
    ./blacklist.nix
    ./alternatives.nix
    ./systemd.nix
  ];

  networking.hostName = lib.mkDefault "lcos";
  system.nixos = {
    distroName = "lcos";
    distroId = "lcos";
    vendorId = "lunduke";
    vendorName = "lunduke";
  };
}