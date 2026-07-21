{ lib, config, ... }: {
  nixpkgs.overlays = [
    (self: super: {
      xwayland-satellite = abort "Lunduke Computer Operating System does not allow XORG compatibility anymore please use wayland";
    })
  ];

  assertions = [
    {
      assertion = !config.services.xserver.enable;
      message = "Lunduke Computer Operating System does not allow XORG anymore please use wayland";
    }
  ];
}