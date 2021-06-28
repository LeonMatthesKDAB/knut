# Settings

Singleton for accessing and editing persistent settings. [More...](#detailed-description)

```qml
import Script 1.0
```

<table>
<tr><td>Since:</td><td>Knut 4.0</td></tr>
</table>

## Methods

| | Name |
|-|-|
|variant |**[setValue](#setValue)**(string path, variant value)|
|variant |**[value](#value)**(string path, variant defaultValue = null)|

## Detailed Description

The settings are stored in a json file, and could be:

- bool
- int
- double
- string
- array<string>

## Method Documentation

#### <a name="setValue"></a>variant **setValue**(string path, variant value)

Adds a new value `value` to the project settings at the given `path`. Returns `true` if the operation succeeded.

#### <a name="value"></a>variant **value**(string path, variant defaultValue = null)

Returns the value of the settings `path`, or `defaultValue` if the settings does not exist.