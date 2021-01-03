# DatabaC
Simple database server for DevOps training. Stores ex. JSON and YAML data. Reading and writing through a RESTful HTTP interface.

### Requirements

Tested with Linux

Building: gcc and make

### Building

```
make
```

### Server usage

The server requires two parameters:

- LISTEN_PORT: The TCP port number the server is going to listen on
- FILE_DIR: Path (relative or absolute) to the directory where the documents are going to be stored

These parameters can either be set as environment variables (called LISTEN_PORT and FILE_DIR) or given as command line parameters during startup.

Give the parameters as command line arguments (port number first and directory second), ex.

```
./databac 8080 /home/iivo/databac
```

To read the values from the environment variables, start the server using

```
./databac use_env
```

### Client usage

You can read with a GET request and write with a POST request. The file names are integers given in the url after the first slash, ex.

```
GET http://localhost:8080/1
```

is going to fetch a file called 1.txt from the FILE_DIR. Similarly a POST request to the same URL will write the body of the request to the file.

## Harjoitukset

### Harjoitus 0:

- Tutustu sovellukseen paikallisella Linux-koneella (myös WSL käy).
- Käännä ja käynnistä palvelin. Tallenna ja lue dokumentteja (tiedostoja) esim. Postmanin avulla.
- Tutustu miten dokumentit tallentuvat tiedostojärjestelmässä.
- Tee DatabaC:n Git-repositoriosta oma forkki, jonne teet muutokset.
	
### Harjoitus 1:

- Kontita sovellus paikallisella Dockerilla (tai Podmanilla tms.) toimivaksi.
- Tarkista, että dokumenttien tallennus ja luku toimivat.
- Dokumenttien ei tarvitse säilyä kontin sammumisen jälkeen.

### Harjoitus 2:

- Asenna sovellus OpenShiftiin.
- Sovelluksen konfiguraatio (kuunteluportti ja tallennushakemisto) tallennetaan ConfigMappiin.
- Nyt dokumenttien täytyy kestää konttien uudelleenkäynnistys.
- Palveluun täytyy saada HTTPS-yhteys.
- Valinnainen lisätehtävä:
	- Muokkaa sovelluskoodia siten, että palvelin jumittuu, kun dokumenttia 666 yritetään lukea.
	- Lisää sovellukselle Liveness-probe.
	
### Harjoitus 3:

- Tee sovellukselle Azure DevOps -putki. 
- Jos mahdollista, tee putkeen automaattikäynnistys lähdekoodien Git-repoa muokattaessa
	- (Azure DevOpsissa bugi GitHub-liipasimissa)
