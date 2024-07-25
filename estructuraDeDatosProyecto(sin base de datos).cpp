#include <iostream>
#include <wchar.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <time.h>
////////////////////////////////////////////////////////////////////////////////////////////////
// COLORES
void Color(int Background, int Text){
	HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
	int New_Color= Text + (Background * 16);
	SetConsoleTextAttribute(Console, New_Color);
}

enum Colors {
 BLACK = 0,
 BROWN = 6,
 LGREY = 7,
 DGREY = 8,
 LGREEN = 10,
 YELLOW = 14,
 WHITE = 15
};
////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
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
void encolar(string _nombre, float _pago_total){
	
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
		
		Color(BLACK, DGREY);
	    cout<<"\t\t|============ LISTA DE NOMBRES DE CLIENTES ==============|"<<endl;
		Color(BLACK, WHITE);
		
		while(p!=NULL){
			cout<<"\t\t Cliente "<<n<<": "<<p->nombre<<endl;
			p=p->sgte;
			n++;
		}
 	}
 	system("pause>nul");
}

//#########################################################################################################
// DESENCOLAR CLIENTE 
void desencolar_cliente(){
	if(cola_cliente == NULL){
		cout<<"\n\t\t[!] Cola vacia"<<endl;
		system("pause>nul");
	}
	else{
		TpCliente del = cola_cliente;
		cola_cliente = cola_cliente->sgte;
		delete(del);
		cout<<"\n\t\t[+] Retirada de cliente"<<endl;
		system("pause>nul");
	}
}


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
void InsertarProducto(){
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
	stringstream temp;	//stringstream: buffer en memória que simula comportarse como un archivo

    temp<<cantidad;
    string cant = temp.str();	// con

	if(cantidad<=0)
	  	return "Agotado";
	else
	   	return cant;	   	
}

//#########################################################################################################
// BUSCAR NODO
int buscar(int _id){
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
// MOSTRAR
void mostrar(){
	system("cls");
	TpNodo p=lista;
	
	cout<<endl;
	Color(BLACK,DGREY);
	cout<<"\t\t\t\t\tPRODUCTOS EXISTENTES:"<<endl<<endl;
	cout<<"\t\tID\t\tNombre\t\t\tPrecio\t\tCantidad\n";
	cout<<"\t\t=================================================================|"<<endl;
	Color(BLACK,WHITE);
	
	while(p != NULL){
		cout<<"\t\t"<<p->id<<"\t\t"<<p->nombre<<"\t\t"<<p->precio<<"\t\t"<<stock(p->cantidad)<<endl;
		p=p->sgte;
	}
	Color(BLACK,DGREY);
	cout<<"\t\t=================================================================|"<<endl;
	Color(BLACK,WHITE);
	
	cout<<"\t\t[+] Productos totales: "<<nr_nodos()<<endl;
	system("pause>nul");
}

//#########################################################################################################
// MODIFICAR UN PRODUCTO
void modificar(){
	int _id, pos;
	TpNodo q=lista;
	
	if (lista == NULL){
    	system("cls");
        cout<<"Lista vacia"<<endl;
    }
    else{
	    mostrar();
	    cout<<endl<<endl;
		cout<<"\t\tIntroduzca el ID de producto > ";cin>>_id;
		
	    pos=buscar(_id);	// posicion nodo a modificare

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
			system("pause>nul");
		}
		else{
			cout<<"\n\t\t[!] Producto NO encontrado";
			system("pause>nul");
		}
	}
}

//#########################################################################################################
// ELIMINAR PRODUCTO
void eliminar_producto(){	
	int _id;
	int cont=nr_nodos(); // para el número de nodos

	TpNodo del=lista;
	TpNodo actual=lista;
	
	if (lista == NULL){
       	cout<<"[!] Lista vacia"<<endl;
       	system("pause");
       	return;
    }
	
	mostrar();
	cout<<endl;
	cout<<"\t\tIntroduzca ID de producto > ";cin>>_id;
	
	int pos=buscar(_id);
	
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
//#########################################################################################################
// COMPRA
void compra(){
    mostrar();
	string cliente, productos[20];	// nombre productos de compra
	int art,c=0;
	int i=1;						// iteracion de compra productos
	float pago=0;
	int _cantidad;
	
    if(lista == NULL){
	    cout<<"\n\t\t[!] NO hay productos para comprar\n";
    }
	else{
		cout<<endl;
		cout<<"\t\t¿Cuántos productos desea comprar?        > "; cin>>art; 
        
        Color(BLACK, BROWN);
		cout<<"\n\n\t\t[+] REALIZANDO COMPRAS"<<endl;
		Color(BLACK, WHITE);
        
		while (i<=art){
			TpNodo actual=lista;

            int _id,pos=0;  
        	cout<<"\t\tIngrese ID de producto que desea comprar > "; cin>>_id;
            
			if(_id <= 0){
				cout<<"\t\t[!] ID NO encontrado"<<endl;
				system("pause");
				return;
			}
	        
			pos=buscar(_id);
	        if(pos <= nr_nodos()){
			 	while(actual->id != _id){
					actual=actual->sgte;
				}
			 	
		        cout<<"\t\t¿Qué cantidad desea?                     > "; cin>>_cantidad;
			       	
			    if(actual->cantidad < _cantidad){
					cout<<"\t\t[!] Cantidad NO disponible"<<endl;
		            i++;
		            continue;	// detener iteracion actual
					//system("pause");
		        }
		           	
		        productos[c]=actual->nombre;	//esto contendrá los nombres de compra de artículos en una matriz
				c++;   
				   
			    pago=pago+(actual->precio * _cantidad); 		//calcular factura
			    actual->cantidad = actual->cantidad - _cantidad;	//actualizar cantidad
			    i++;
	     	}
	    	else{    
				cout<<"\t\t[!] Producto NO disponible"<<endl;
				system("pause>nul");
				i++;
				continue;	//return antes
		    }
		}

        cout<<"\n\t\tIntroduzca su nombre                     > "; cin>>cliente;
		cout<<endl<<endl;
        cout<<"\t\t[+] Usted ha comprado  > ";
       
	   	for(int i=0;i<art;i++){
        	cout<<productos[i]<<", ";	// imprmir nombre de productos
       	}
       	
		encolar(cliente,pago);
		
		cout<<endl;
	    cout<<"\t\t[+] Precio a pagar     > "<<pago<<endl;
    }
}


//#########################################################################################################
//#########################################################################################################
// CARGAR PRDUCTOS DE UN ARCHIVO
void cargar_productos(){
	char   _nombre[20], _precio[10];	
	string _id, _cantidad;
	
	ifstream archivo;
	archivo.open("productos.txt", ios::in);
	
	while(!archivo.eof()){
		archivo >>_id >>_nombre >>_precio >>_cantidad;
		
		if(!archivo.eof()){
			TpNodo nuevo = new(struct nodo);
			
			nuevo->id       = atoi(_id.c_str());		//string a entero
			nuevo->nombre   = _nombre;
			nuevo->precio   = strtod(_precio,NULL);		// arreglo char a flotante
			nuevo->cantidad = atoi(_cantidad.c_str());
			
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
				//temporal=nuevo;
			}		
		}
	}
	archivo.close();
	cout<<"\t\t[+] Productos cargados"<<endl;
	system("pause>nul");
}

//#########################################################################################################
// MENU DE ADMINISTRADOR
void administrador(){

	int opc;
	
	do{
		system("cls");
		Color(BLACK,BROWN);
		cout<<"\t\t============================================"<<endl;
		cout<<"\t\t|          Portal de administrador         |"<<endl;
		cout<<"\t\t============================================"<<endl<<endl;
		Color(BLACK, WHITE);
	
		cout<<"\t\t1.- INSERTAR nuevo producto"<<endl;
		cout<<"\t\t2.- MOSTRAR productos"<<endl;
		cout<<"\t\t3.- MODIFICAR producto"<<endl;
		cout<<"\t\t4.- ELIMINAR un producto"<<endl;
		cout<<"\t\t5.- MOSTRAR cola de clientes"<<endl;
		cout<<"\t\t6.- DESENCOLAR cliente"<<endl;
		cout<<"\t\t7.- CARGAR productos"<<endl;
		cout<<"\t\t0.- MENU PRINCIPAL"<<endl<<endl;
		
		Color(BLACK, YELLOW); cout<<"\t\tOpción > "; Color(BLACK, WHITE); 
		cin>>opc;
		
		switch(opc){
			case 1:{
				InsertarProducto();
				system("pause>nul");
				break;
			}
			case 2:{
				mostrar();
				break;
			}
			case 3:{
				modificar();
				break;
			}
			case 4:{
				eliminar_producto();
				system("pause>nul");
				break;
			}
			case 5:{
				mostrar_cola();
				break;
			}
			case 6:{
				desencolar_cliente();
				break;
			}
			case 7:{
				cargar_productos();
				break;
			}
		}//switch
	}while(opc!=0);
	
}

//#########################################################################################################
// FUNCION MAIN
int main(){
	setlocale(LC_ALL, "");
	
	int opc;
	
	while(opc != 3){
	    system("cls");
		Color(BLACK, LGREEN); cout<<"\n\t\t|--------< MENÚ PRINCIPAL >-----------|\n"; Color(BLACK, WHITE);
	    
		Color(BLACK, BROWN);
	    cout<<"\t\t\t                  ___"<<endl;
		cout<<"\t\t\t                 /  |"<<endl;
		cout<<"\t\t\t  ______________/   --"<<endl;
		cout<<"\t\t\t |___/__ /___/_|     "<<endl;     
		cout<<"\t\t\t |__/___/___/__|     "<<endl;     
		cout<<"\t\t\t |_/___/___/___|     "<<endl;     
		cout<<"\t\t\t    _______/         "<<endl;
		cout<<"\t\t\t     O   O           "<<endl;
		Color(BLACK, WHITE);
	    
	    cout<<"\n\n";
		cout<<"\t\t 1) Administrador de SuperMercado"<<endl;
		cout<<"\t\t 2) Ingresar Cliente"<<endl;
		cout<<"\t\t 3) Salir"<<endl<<endl;
	
		Color(BLACK, YELLOW); cout<<"\t\t Opción > "; Color(BLACK, WHITE);
		cin>>opc;
	
		switch(opc){
		case 1:
			administrador();
			system("cls");
			break;
		case 2:
			cout<<endl<<endl;
			compra();
			system("pause>nul");
		    break;
	
	    case 3:
	    	cout<<"\n\n\t\tGracias por su visita :)"<<endl<<endl;

	    	break;
		}
	}
	
	return 0;
}

//#########################################################################################################
