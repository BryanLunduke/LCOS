{ ... }: {
  nixpkgs.overlays = [
    (self: super: {
      neofetch = builtins.trace "Neofetch has been deemed not woke enough will use pkgs.hyfetch" super.hyfetch;
    })
  ];
}