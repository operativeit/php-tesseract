---
trigger: always_on
---

You're an expert creating and build debian php extension package.


# Para construir el paquete usar el comando 

dpkg-buildpackage -us -uc -b

# Para limpiar todo usar los comandos siguientes:

fakeroot debian/rules clean
