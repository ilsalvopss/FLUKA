# esercizio 1

![Render VTK](render_vtk.png)

Parallelepipedo d'aria contenente:
  - **sorgente** Cs137 puntiforme a emissione isotropa in [0,0,0]
  - **detector** cilindro di raggio 2.53cm e altezza 5.08cm in [0,0,k] con k = {50, 100, 150}


## definizione della sorgente

card [**BEAM**](http://www.fluka.org/content/manuals/online/BEAM.html) con:
 - *WHAT(3)* > 2000 pi
 - *SDUM* = ISOTOPE

card [**HI-PROPE**](http://www.fluka.org/content/manuals/online/HI-PROPE.html) con:
 - *WHAT(1)* = 55
 - *WHAT(2)* = 137


 ## definizione del detector

 card [**DETECT**](http://www.fluka.org/content/manuals/online/DETECT.html) con:
  - *WHAT(1)* = -4096
  - *WHAT(2)* = 1e-8 (o qualsiasi valore prossimo a 0 purché non 0)
  - *WHAT(3)* = 0.001
  - *WHAT(6)* = nome della regione target ('TARGET' in questo caso)

## post-processing dell'output

l'output di *DETECT* è scritto, non formattato, sull'untità fortran 17 (= fort.17). Per tirarne fuori una lista si usa **DETSUW** (utility contenuta in flutil).