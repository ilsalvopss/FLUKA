# fluka per spettroscopia gamma

## card globali per tutti gli esercizi

- card [**DEFAULTS**](http://www.fluka.org/content/manuals/online/DEFAULTS.html) con:
  - *SDUM* = `PRECISO`
- card [**PHYSICS**](http://www.fluka.org/content/manuals/online/PHYSICS.html) con:
  - *SDUM* = `COALESCE`
  - *WHAT(1)* = `1`
- card [**PHYSICS**](http://www.fluka.org/content/manuals/online/PHYSICS.html) con:
  - *SDUM* = `EVAPORAT`
  - *WHAT(1)* = `3`
- card [**PHOTONUC**](http://www.fluka.org/content/manuals/online/PHOTONUC.html) con:
  - *WHAT(1)* = `1`
- card [**EMF**](http://www.fluka.org/content/manuals/online/EMF.html)
- card [**EMFCUT**](http://www.fluka.org/content/manuals/online/EMFCUT.html) con:
  - *SDUM* = `PROD-CUT`
  - *WHAT(1)* = `-1e-6`
  - *WHAT(2)* = `1e-7`
  - *WHAT(3)* = `1`
  - *WHAT(4)* = `HYDROGEN`
  - *WHAT(5)* = `@LASTMAT`
- card [**EMFRAY**](http://www.fluka.org/content/manuals/online/EMFRAY.html) con:
  - *WHAT(1)* = `1`
  - *WHAT(2)* = `AIR` (o la prima regione in cui attivare)
  - *WHAT(3)* = `TARGET` (o l'ultima regione in cui attivare)
- card [**EMFFLUO**](http://www.fluka.org/content/manuals/online/EMFFLUO.html) con:
  - *WHAT(1)* = `1`
  - *WHAT(2)* = `HYDROGEN`
  - *WHAT(3)* = `@LASTMAT`
- card [**RADDECAY**](http://www.fluka.org/content/manuals/online/RADDECAY.html) con:
  - *WHAT(1)* = `2`
