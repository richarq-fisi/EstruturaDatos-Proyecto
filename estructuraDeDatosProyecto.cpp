#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <mariadb/mysql.h>

#define COLOR_VERDE   "\x1b[32m"
#define COLOR_AMARILLO  "\x1b[33m"
#define COLOR_AMARILLO_I  "\033[1;93m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_PLOMO    "\e[0;37m\033[1m"

using namespace std;

MYSQL *con = mysql_init(NULL);

//#########################################################################################################
// NODO : datos de un producto
struct nodo{
	int    id;
	string nombre;
	float  precio;
	int    cantidad;
	struct nodo *sgte;
};

//#########################################################################################################
// Definiendo un tipo de dato tipo struct nodo
typedef struct nodo *TpNodo;
TpNodo lista=NULL, temporal=NULL;		// temp (usado en funcion cargar_productos)

//#########################################################################################################
//CLIENTE
struct cliente{
	string nombre;
	float pago_total;
	struct cliente *sgte;
};

typedef struct cliente *TpCliente;
TpCliente cola_cliente = NULL;

//#########################################################################################################
// ENCOLAR CLIENTE Y GUARDAR REGISTRO EN ARCHIVO
void encolar_cliente(string _nombre, float _pago_total){
	
	time_t t_actual = time(0);
	struct tm tstruct;
	char tiempo[80];
	tstruct = *localtime(&t_actual);
	strftime(tiempo, sizeof(tiempo), "Fecha: %Y-%m-%d\t\tHora: %X", &tstruct);

	TpCliente temp = new(struct cliente), p=cola_cliente;
	ofstream archivo;
	archivo.open("registro_clientes.txt",ios::app);

	temp->nombre=_nombre;
	temp->pago_total=_pago_total;
	temp->sgte=NULL;


	archivo<<tiempo<<"\t\tCliente: "<<_nombre<<"\t\tPago_Total: S/."<<_pago_total<<endl;

	if( cola_cliente == NULL ){	//Si se inserta el primer elemento/nodo
  		cola_cliente=temp;
	}
	else{
  		while(p->sgte != NULL){
			p=p->sgte;
		}
		p->sgte=temp;
	}
	archivo.close();
}

//#########################################################################################################
//MOSTRAR COLA
void mostrar_cola(){
 	if (cola_cliente == NULL){
		cout<<"\n\t\t[!] Cola vacia"<<endl;
	}
 	else{
	  	TpCliente p=cola_cliente;
		int n=1;

		system("cls");
		cout<<endl<<endl;

		cout<<COLOR_PLOMO;
	    cout<<"\t\t|============ LISTA DE NOMBRES DE CLIENTES ==============|"<<endl;
		cout<<COLOR_RESET;

		while(p!=NULL){
			cout<<"\t\t Cliente "<<n<<": "<<p->nombre<<endl;
			p=p->sgte;
			n++;
		}
 	}
 	system("pause");
}

//#########################################################################################################
// DESENCOLAR CLIENTE
void desencolar_cliente(){
	if(cola_cliente == NULL){
		cout<<"\n\t\t[!] Cola vacia"<<endl;
		system("pause");
	}
	else{
		TpCliente del = cola_cliente;
		cola_cliente = cola_cliente->sgte;
		delete(del);
		cout<<"\n\t\t[+] Retirada de cliente"<<endl;
		system("pause");
	}
}

//#########################################################################################################
//#########################################################################################################
// CREACION DE NODO
TpNodo CrearNodo(){
		TpNodo nuevo = new(struct nodo);
		system("cls");
		cout<<endl<<endl;
		cout<<"\t\tIngresar ID de producto       > ";cin>>nuevo->id;
		cout<<"\t\tIngresar nombre de producto   > ";cin>>nuevo->nombre;
		cout<<"\t\tIngresar precio de producto   > ";cin>>nuevo->precio;
		cout<<"\t\tIngresar cantidad de producto > ";cin>>nuevo->cantidad;
		nuevo->sgte=NULL;
		return nuevo;
}

//#########################################################################################################
//CANTIDAD DE NODOS EN UNA LISTA
int nr_nodos(){
	int nodos=0;
	TpNodo p=lista;
	while(p != NULL){
		p=p->sgte;
		nodos=nodos+1;
	}
	return nodos;
}

//#########################################################################################################
// INSERTAR UN PRODUCTO
void insertar_producto(){
	TpNodo p=lista, q=CrearNodo();

	if(lista==NULL){
		lista=q;
		temporal=lista;		// si no igualo a lista temp seria NULL
	}
	else{
		while(p->sgte != NULL)
			p=p->sgte;
		p->sgte=q;
	}
	cout<<endl;
	cout<<"\t\t[+] Producto insertado"<<endl;
}

//#########################################################################################################
// STOCK DE CANTIDAD DE PRODUCTOS
string stock(int cantidad){
	stringstream temp;	//stringstream: buffer en memoria que simula comportarse como un archivo

    temp<<cantidad;
    string cant = temp.str();	// con

	if(cantidad<=0)
	  	return "Agotado";
	else
	   	return cant;
}

//#########################################################################################################
// BUSCAR NODO
int buscar_nodo(int _id){
 	int cont=1;
 	TpNodo p=lista;
 	while(p != NULL){
 		if(p->id == _id)
 			break;
 		else
 			cont++;
 			p=p->sgte;
	}
 	return cont;	// devolver pos. del nodo buscado
 }

//#########################################################################################################
// CARGAR REGISTROS DE PRODUCTOS	(BASE DE DATOS)
void cargar_db(){
	TpNodo p=lista;
	MYSQL_ROW fila, fil;
	MYSQL_RES *resultado, *res;

	///////////	OBTENER NUMERO DE REGISTROS
	string nr_filas="SELECT COUNT(*) FROM  Productos";
	const char *b=nr_filas.c_str();

	mysql_query(con, b);
	res=mysql_store_result(con);//Devuelve un resultado almacenado en búfer de la última consulta ejecutada.
	fil=mysql_fetch_row(res);

	int cont=atoi(fil[0]);		//puntero char a int
	///////////

	string consulta="SELECT * FROM Productos";
	const char *c=consulta.c_str();
	mysql_query(con, c);
	resultado=mysql_store_result(con);

	while(cont > 0){
	TpNodo nuevo = new(struct nodo);

		while(fila=mysql_fetch_row(resultado)){
			//	id      : fila[0]
			//	nombre  : fila[1]
			//	precio  : fila[2]
			// cantidad : fila[3]
			nuevo->id      = atoi(fila[0]);					//puntero char a int
			nuevo->nombre  = fila[1];						//puntero char a string
			nuevo->precio  = (float)strtod(fila[2],NULL);	//puntero char a float
			nuevo->cantidad= atoi(fila[3]);					//puntero char a int
			nuevo->sgte    = NULL;

			if(lista==NULL){
				lista=nuevo;
				lista->sgte=NULL;
				temporal=lista;
				cont--;
			}else{
				temporal=lista;
				while(temporal->sgte != NULL){
					temporal=temporal->sgte;
				}
				temporal->sgte=nuevo;
				nuevo->sgte=NULL;
				temporal=nuevo;
				cont--;
			}
			break;
		}
	}
}

//#########################################################################################################
// INSERTAR REGISTROS EN LA TABLA PRODUCTOS
void guardar_db(){
	mysql_query(con, "DROP TABLE IF EXISTS Productos");
	mysql_query(con, "CREATE TABLE Productos(id INT NOT NULL, nombre VARCHAR(20) NOT NULL, precio FLOAT NOT NULL, cantidad VARCHAR(20) NOT NULL);");

	TpNodo p=lista;

	while(p != NULL){
		stringstream buff, buff2;	//buffer en memória que simula comportarse como un archivo
		buff <<  p->id;		// int a string
		buff2 << p->precio;	// float a string

		string consulta="INSERT INTO Productos VALUES("  +
						buff.str()         + ", '"  +
						p->nombre          + "', '" +
						buff2.str()        + "', '"  +
						stock(p->cantidad) + "');";
		//INSERT INTO Productos VALUES(id, nombre, precio, cantidad);
		const char* insertar=consulta.c_str();

		mysql_query(con, insertar);
		p=p->sgte;
	}
}

//#########################################################################################################
// MOSTRAR
void mostrar(){
	system("cls");
	TpNodo p=lista;

	cout<<endl;
	cout<<COLOR_PLOMO;
	cout<<"\t\t\t\t\tPRODUCTOS EXISTENTES:"<<endl<<endl;
	cout<<"\t\tID\t\tNombre\t\t\tPrecio\t\tCantidad\n";
	cout<<"\t\t=================================================================|"<<endl;
	cout<<COLOR_RESET;

	while(p != NULL){
		cout<<"\t\t"<<p->id<<"\t\t"<<p->nombre<<"\t\t"<<p->precio<<"\t\t"<<stock(p->cantidad)<<endl;
		p=p->sgte;
		system("sleep 0.1");
	}

	cout<<COLOR_PLOMO;
	cout<<"\t\t=================================================================|"<<endl;
	cout<<COLOR_RESET;

	cout<<"\t\t[+] Productos totales: "<<nr_nodos()<<endl;
	system("pause");
}

//#########################################################################################################
// MODIFICAR UN PRODUCTO
void modificar_producto(){
	int _id, pos;
	TpNodo q=lista;

	if (lista == NULL){
        cout<<"\n\t\t[!] Lista vacia"<<endl;
        system("pause");
    }
    else{
	    mostrar();
	    cout<<endl<<endl;
		cout<<"\t\tIntroduzca el ID de producto > ";cin>>_id;

	    pos=buscar_nodo(_id);	// posicion nodo a modificar

	    if(pos <= nr_nodos()){
			while(q->id != _id){
				q=q->sgte;
			}
			cout<<endl;
			cout<<"\t\tID antiguo             : "<<q->id<<endl;;
			cout<<"\t\tNombre antiguo         : "<<q->nombre<<endl;
			cout<<"\t\tPrecio antiguo         : "<<q->precio<<endl;
			cout<<"\t\tCantidad antigua       : "<<q->cantidad;

			cout<<endl<<endl;
			cout<<"\t\tIngrese nuevo ID       > ";cin>>q->id;
			cout<<"\t\tIngrese nuevo nombre   > ";cin>>q->nombre;
			cout<<"\t\tIngrese nuevo precio   > ";cin>>q->precio;
			cout<<"\t\tIngrese nueva cantidad > ";cin>>q->cantidad;

			cout<<endl;
			cout<<"\t\t[+] Modificación exitosa"<<endl;
			system("pause");
		}
		else{
			cout<<"\n\t\t[!] Producto NO encontrado"<<endl;
			system("pause");
		}
	}
}

//#########################################################################################################
// ELIMINAR PRODUCTO
void eliminar_producto(){
	int _id;
	int cont=nr_nodos(); // para el numero de nodos

	TpNodo del=lista;
	TpNodo actual=lista;

	if (lista == NULL){
       	cout<<"\t\t[!] Lista vacia"<<endl;
       	system("pause");
       	return;
    }

	mostrar();
	cout<<endl;
	cout<<"\t\tIntroduzca ID de producto > ";cin>>_id;

	int pos=buscar_nodo(_id);

	if(pos<=cont){
		while(del->id != _id){
			actual=del;
			del=del->sgte;
		}

		actual->sgte=del->sgte;

		cout<<"\t\t[!] Producto "<<del->nombre<<" ha sido eliminado"<<endl;
		delete(del);
	}
	else{
		cout<<"\n\t\t[!] ID NO encontrado"<<endl<<endl;
	}
}

//#########################################################################################################
// COMPRA
void comprar(){
    mostrar();
	string cliente;				// nombre del cliente
	string nombre_producto[20];	// nombre de productos de compra
	int art;					// productos a comprar
	int c=0;					// iterar array nombre_productos[]
	int inc=1;					// iteracion de compra productos
	float pago=0;				// pago total
	int _cantidad;				// cantidad por producto
	int _id;					// ID de producto a comprar
	int pos=0;					// posicion del producto a comprar
	int prod_cantidad[20];		// cantidad comprada por producto

    if(lista == NULL){
	    cout<<"\n\t\t[!] NO hay productos para comprar\n";
    }
	else{
		cout<<endl;
		cout<<"\t\t¿Cuántos productos desea comprar?        > "; cin>>art;

        cout<<COLOR_AMARILLO_I;
		cout<<"\n\n\t\t[+] REALIZANDO COMPRAS"<<endl;
		cout<<COLOR_RESET;

		while (inc<=art){
			TpNodo actual=lista;

            //int _id,pos=0;
            cout<<endl;
        	cout<<"\t\tIngrese ID de producto que desea comprar > "; cin>>_id;

			pos=buscar_nodo(_id);
	        if(pos <= nr_nodos()){
			 	while(actual->id != _id){
					actual=actual->sgte;
				}

		        cout<<"\t\t¿Qué cantidad desea?                     > "; cin>>_cantidad;

			    if(actual->cantidad < _cantidad){
					cout<<"\t\t[!] Cantidad NO disponible"<<endl;
		            continue;	// detener iteracion actual
		        }

		        nombre_producto[c]=actual->nombre;	//esto contendra los nombres de compra de articulos en una matriz
				prod_cantidad[c]=_cantidad;
				c++;

			    pago = pago + (actual->precio * _cantidad); 		//calcular el pago
			    actual->cantidad = actual->cantidad - _cantidad;	//actualizar cantidad
			    inc++;
	     	}
	    	else{
				cout<<"\t\t[!] Producto NO disponible"<<endl;
				//continue;
		    }
		}

        cout<<"\n\t\tIntroduzca su nombre                     > "; cin>>cliente;
		cout<<endl<<endl;

		cout<<"\t\t================================================================="<<endl;
        cout<<"\t\t[+] USTED HA COMPRADO: ";

	   	for(int i=0;i<art;i++){
        	cout<<endl<<"\t\t"<<nombre_producto[i]<<"\tx\t"<<prod_cantidad[i];	// imprmir nombre de productos
       	}

		encolar_cliente(cliente,pago);

		cout<<endl<<endl;
	    cout<<"\t\t[+] PRECIO A PAGAR:"<<endl;
	    cout<<"\t\tS/. "<<pago<<endl;
    }
}

//#########################################################################################################
// CARGAR PRDUCTOS DE UN ARCHIVO
void cargar_productos(){
	char _nombre[20], _precio[10];
	string _id, _cantidad;

	ifstream archivo;
	archivo.open("productos.txt", ios::in);

	while(!archivo.eof()){
		archivo >>_id >>_nombre >>_precio >>_cantidad;	// guardando en las variables

		if(!archivo.eof()){
			TpNodo nuevo = new(struct nodo);

			nuevo->id=atoi(_id.c_str());				//string a entero
			nuevo->nombre=_nombre;
			nuevo->precio=strtod(_precio,NULL);			// arreglo char a flotante
			nuevo->cantidad=atoi(_cantidad.c_str());

			if(lista==NULL){
				lista=nuevo;
				lista->sgte=NULL;
				temporal=lista;
			}else{
				temporal=lista;
				while(temporal->sgte != NULL){
					temporal=temporal->sgte;
				}
				temporal->sgte=nuevo;
				nuevo->sgte=NULL;
				temporal=nuevo;
			}
		}
	}
	archivo.close();
	cout<<"\n\t\t[+] Productos cargados"<<endl;
	system("pause");
}

//#########################################################################################################
// MENU DE ADMINISTRADOR
void administrador(){
	int opc;

	do{
		guardar_db();
		system("cls");
		cout<<endl;
		cout<<COLOR_AMARILLO_I;
		cout<<"\t\t============================================"<<endl;
		cout<<"\t\t|          Portal de administrador         |"<<endl;
		cout<<"\t\t============================================"<<endl<<endl;
		cout<<COLOR_RESET;

		cout<<"\t\t1.- INSERTAR nuevo producto"<<endl;
		cout<<"\t\t2.- MOSTRAR productos"<<endl;
		cout<<"\t\t3.- MODIFICAR producto"<<endl;
		cout<<"\t\t4.- ELIMINAR un producto"<<endl<<endl;
		cout<<"\t\t5.- MOSTRAR cola de clientes"<<endl;
		cout<<"\t\t6.- DESENCOLAR cliente"<<endl;
		cout<<"\t\t7.- CARGAR productos"<<endl;
		cout<<"\t\t0.- MENU PRINCIPAL"<<endl<<endl;

		cout<<COLOR_AMARILLO; cout<<"\t\tOpción > ";cout<<COLOR_RESET;
		cin>>opc;

		switch(opc){
			case 1:
				insertar_producto();
				system("pause");
				break;
			case 2:
				mostrar();
				break;
			case 3:
				modificar_producto();
				break;
			case 4:
				eliminar_producto();
				system("pause");
				break;
			case 5:
				mostrar_cola();
				break;
			case 6:
				desencolar_cliente();
				break;
			case 7:
				cargar_productos();
				break;
		}//switch
	}while(opc!=0);

}

//#########################################################################################################
// FUNCION MAIN

int main(){

	if (mysql_real_connect(con, "localhost", "test123", "test123", "SuperMercado", 3306, NULL, 0) == NULL){
   		cout<<"\t\t[!] Error en la conexión"<<endl;
   		system("pause");
   		return 0;
	}

	cargar_db();

	int opc;

	while(opc != 3){
	    system("cls");
		cout<<COLOR_VERDE;
		cout<<"\n\t\t|--------< MENÚ PRINCIPAL >-----------|\n";
		cout<<COLOR_RESET;

		cout<<COLOR_AMARILLO_I;
	    cout<<"\t\t\t                  ___"<<endl;
		cout<<"\t\t\t                 /  |"<<endl;
		cout<<"\t\t\t  ______________/   --"<<endl;
		cout<<"\t\t\t |___/__ /___/_|     "<<endl;
		cout<<"\t\t\t |__/___/___/__|     "<<endl;
		cout<<"\t\t\t |_/___/___/___|     "<<endl;
		cout<<"\t\t\t    _______/         "<<endl;
		cout<<"\t\t\t     O   O           "<<endl;
		cout<<COLOR_RESET;

	    cout<<"\n\n";
		cout<<"\t\t 1) Administrador de SuperMercado"<<endl;
		cout<<"\t\t 2) Ingresar Cliente"<<endl;
		cout<<"\t\t 3) Salir"<<endl<<endl;

		cout<<COLOR_AMARILLO; cout<<"\t\t Opción > "; cout<<COLOR_RESET;
		cin>>opc;

		switch(opc){
			case 1:
				administrador();
				system("cls");
				break;
			case 2:
				cout<<endl<<endl;
				comprar();
				system("pause");
			    break;

		    case 3:
		    	cout<<"\n\n\t\tGracias por su visita :)"<<endl<<endl;
		    	mysql_close(con);
		    	break;
		}
	}
	return 0;
}

