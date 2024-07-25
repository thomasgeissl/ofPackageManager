# json interface

The package manager provides a json interface.

## actions

Actions consist of a required type and an optinal payload. Responses consit of a success property and optional a payload with additional information.

### getting version

#### request

```json
{
  "type": "GETVERSION"
}
```

#### response

```json
{ "payload": { "major": 0, "minor": 0, "patch": 16 }, "success": true }
```

### getting list of available packages in database

#### request

```json
{
  "type": "GETAVAILABLEPACKAGES",
  "payload": {
    "config": {
      "localAddonsPath": "local_addons",
      "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      "packagesPath": "/Users/thomas.geissl/.ofPackages"
    }
  }
}
```

#### response

```json
{
  "payload": {
    "data": []
  },
  "success": true
}
```

### installing a package by id

#### request

```json
{
  "type": "INSTALLPACKAGEBYID",
  "payload": {
    "id": "ofxMidi",
    "destination": "local_addons",
    "config": {
      "localAddonsPath": "local_addons",
      "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      "packagesPath": "/Users/thomas.geissl/.ofPackages"
    }
  }
}
```

#### response

```json
{
  "payload": {
    "package": {
      "checkout": "f1d92fd8e",
      "path": "local_addons/ofxMidi",
      "url": "https://github.com/danomatika/ofxMidi.git"
    }
  },
  "success": true
}
```

### install package by id with specified checkout (commit hash or tag)

#### request

```json
{
  "type": "INSTALLPACKAGEBYID",
  "payload": {
    "id": "ofxLua",
    "checkout": "1.3.0",
    "destination": "local_addons",
    "config": {
      "localAddonsPath": "local_addons",
      "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      "packagesPath": "/Users/thomas.geissl/.ofPackages"
    }
  }
}
```

#### response

```json
{
  "payload": {
    "package": {
      "checkout": "d792bb700",
      "path": "local_addons/ofxLua",
      "url": "https://github.com/danomatika/ofxLua.git"
    }
  },
  "success": true
}
```

### install all packages listed in addons.make

#### request

```json
{
  "type": "INSTALL",
  "config": {
    "localAddonsPath": "local_addons",
    "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
    "packagesPath": "/Users/thomas.geissl/.ofPackages"
  }
}
```

#### response

```json
{ "success": true }
```

### adding package to the addons.make file

#### request

```json
{
  "type": "ADDPACKAGETOADDONSMAKEFILE",
  "payload": {
    "package": {
      "checkout": "f1d92fd8e",
      "path": "local_addons/ofxMidi",
      "url": "https://github.com/danomatika/ofxMidi.git"
    },
    "config": {
      "localAddonsPath": "local_addons",
      "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      "packagesPath": "/Users/thomas.geissl/.ofPackages"
    }
  }
}
```

#### response

```json
{ "success": true }
```

### searching package in database by id

#### request

```json
{
  "type": "SEARCHPACKAGEINDATABASEBYID",
  "payload": {
    "id": "ofxCv",
    "config": {
      "localAddonsPath": "local_addons",
      "ofPath": "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      "packagesPath": "/Users/thomas.geissl/.ofPackages"
    }
  }
}
```

#### response

```json
{ "payload": { "data": { "items": [] } }, "success": true }
```

### searching package on github by name

#### request

```json
{
  "type": "SEARCHPACKAGEONGITHUBBYNAME",
  "payload": {
    "id": "ofxCv"
  }
}
```

#### response

```json
{ "payload": { "data": {} }, "success": true }
```

### searching package on github by user

#### request

```json
{
  "type": "SEARCHPACKAGEONGITHUBBYUSER",
  "payload": {
    "user": "bakercp"
  }
}
```

#### response

```json
{ "payload": { "data": {} }, "success": true }
```

## examples

- a node.js example can be found in scripts/test/index.js
- terminal: `ofPackageManager "{\"type\": \"GETVERSION\"}"`
