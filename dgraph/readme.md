[DGraph]
======================

[TOC]

### Instalación

Necesitamos librerías como SQLite, Boost y GCC-7

#### Requerimientos

```bash
$ sudo apt-get update
$ sudo apt-get install sqlite3
$ sudo apt-get install libsqlite3-dev
$ sudo apt-get install sqlitebrowser
$ sudo apt-get install libboost-all-dev
```

#### Compilación

```bash
$ cd folder name
$ mkdir db
$ make clean
$ make
```

### Ejecución

#### Estático [2 Esclavos]

##### Esclavo/SQLite

```bash
$ # ./slave.out port ip db-name
$ ./slave.out 8000 127.0.0.1 slave1
  Created Database Successfully!
  
  --------
$ ./slave.out 8004 127.0.0.1 slave2
  Created Database Successfully!
```
##### Cliente/Maestro
```c++
void TFront::set_slaves(){
	/*Dinamico*/

    /*Estático*/
	m_slaves.push_back({8000, "127.0.0.1"});
	m_slaves.push_back({8004, "127.0.0.1"});
}
```

Ejecutar

```bash
$ # ./front.out port ip
$ ./front.out 6666 127.0.0.1
  sarah: 
```

#### Estático [N Esclavos]

##### Esclavo/SQLite

```bash
# N = 5
$ ./slave.out 8000 127.0.0.1 slave1
  Created Database Successfully!  
  --------
$ ./slave.out 8004 127.0.0.1 slave2
  Created Database Successfully!
  --------
$ ./slave.out 8008 127.0.0.1 slave3
  Created Database Successfully!
  --------
$ ./slave.out 8012 127.0.0.1 slave4
  Created Database Successfully!
  --------
$ ./slave.out 8016 127.0.0.1 slave5
  Created Database Successfully!
```

##### Cliente/Maestro

```c++
void TFront::set_slaves(){
	/*Dinamico*/
	int ns;
	cout << "Slaves Number: "; cin >> ns;

	int port;
	string ip;
	for(int i=0; i<ns; i++){
		cout << "  Slave " << i+1 << "\n";
		cout << "    Port: "; cin >> port;
		cout << "    Ip: "; cin >> ip;
		cout << "\n";
		m_slaves.push_back({port, ip});
	}
    
    /*Estático*/
}
```
Ejecutar
```bash
$ # ./front.out port ip
$ ./front.out 6666 127.0.0.1
  Slaves Number: 5
  Slave 1
    Port: 8000 
    Ip: 127.0.0.1

  Slave 2
    Port: 8004
    Ip: 127.0.0.1

  ........

  Slave 5
    Port: 8016
    Ip: 127.0.0.1

```
### Comando

#### Insertar

Insertar un nodo [R]

```bash
  sarah: I N Nodo Value
```

Insertar una relación nodo-nodo [R]

```bash
  sarah: I R Nodo1 Nodo2
```

#### Borrar

Insertar un nodo

```bash
  sarah: I N Name Value
```

Insertar un nodo

```bash
  sarah: I N Name Value
```

#### Selección

Extraer todo los nodos existentes

```bash
  sarah: A N
```
Extraer todo las relaciones existentes

```bash
  sarah: A R
```

Extraer información de un nodo en especifico

```bash
  sarah: Q N Nodo
```

Extraer las relaciones de un nodo

```bash
  sarah: Q R Nodo
```

Extraer todo los nodos

```bash
  sarah: A N
```

#### Borrado
Borrar un nodo y sus relaciones

```bash
  sarah: D N Nodo
```
#### Salir

```bash
  sarah: exit
  sarah: Exit
  sarah: q
```

### Ejemplo

#### Grafo

![](/home/maldonado/Network/dgraph/images/graph.png)

Grafo simple generado para prueba

#### Estático

Cargaremos el grafo de la imagen a través de nuestro cliente.

##### Cliente
Debemos de tener en la carpeta db el archivo init.txt

```
I N U 1
I N B 2
I N A 3
I N D 4
I N C 5
I N T 6
I N E 7
I N W 8
I N Z 9
I N X 10
I N Y 11
I R U B
I R B A
I R B D
I R D C
I R D T
I R D E
I R C E
I R E W
I R X Y
```

Ejecutar

```bash
$ ./front.out 6666 127.0.0.1
  sarah: random
```
* * *
[Percy Maldonado Quispe UCSP](https://github.com/maldonadoq)