#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;


struct Regla {
    string antecedente;
    string consecuente;
    double factorCerteza;
};

struct Hecho {
    string hecho;
    double factorCerteza;

};



// Funcion para eliminar el algoritmo caracter de una cadena
void eliminarUltimoCaracter(string &cadena) {
    if (!cadena.empty()) {
        cadena.erase(cadena.size() - 1);
    }
}



//Lee el fichero que contine la BC
vector<Regla> lecturaBC(const char* BC){
	ifstream archivo;
	string texto;
	string aux;
	archivo.open(BC, ios::in);

	if(archivo.fail()){
		cerr << "No se pudo abrir el archivo." << endl;
	}

	int numeroReglas;
    archivo >> numeroReglas;
    archivo.ignore();  // Limpiar el buffer de entrada

    vector<Regla> reglas;

     for (int i = 0; i < numeroReglas; ++i) {
        string entrada;
        getline(archivo, entrada);

        istringstream stream(entrada);
        string token;
        Regla nuevaRegla;

         // Almacenar todo antes de "Entonces"
        while (stream >> token && token != "Entonces") {
            if (!nuevaRegla.antecedente.empty()) {
                nuevaRegla.antecedente += " ";
            }
            nuevaRegla.antecedente += token;
        }

		stream >> nuevaRegla.consecuente;
		nuevaRegla.consecuente = nuevaRegla.consecuente.erase(nuevaRegla.consecuente.size()-1);
		stream >> aux;
		aux = aux.erase(0, 3);
		istringstream ss(aux);
		ss >> nuevaRegla.factorCerteza;

        // Agregar la nueva regla al vector
        reglas.push_back(nuevaRegla);
    }

	archivo.close();

	return reglas;
}



//Lee el fichero que contine la BH
vector<Hecho> lecturaBH(const char* BH, string &obj){
	ifstream archivo;
	string texto;
	string aux;

	archivo.open(BH, ios::in);

	if(archivo.fail()){
		cerr << "No se pudo abrir el archivo." << endl;

	}

	int numeroReglas;
    archivo >> numeroReglas;
    archivo.ignore();  // Limpiar el buffer de entrada

    vector<Hecho> hechos;

     for (int i = 0; i < numeroReglas; ++i) {
        string entrada;
        getline(archivo, entrada);

        istringstream stream(entrada);
        string token;
        Hecho nuevoHecho;

        stream >> nuevoHecho.hecho;
        nuevoHecho.hecho = nuevoHecho.hecho.erase(nuevoHecho.hecho.size()-1);

        stream >> aux;
		aux = aux.erase(0, 3);
		istringstream ss(aux);
		ss >> nuevoHecho.factorCerteza;

        // Agregar la nueva regla al vector
        hechos.push_back(nuevoHecho);
    }


    for (int i = numeroReglas; i < numeroReglas+2; ++i) {
        string entrada;
        
        getline(archivo, entrada);

        istringstream stream(entrada);

        string token;

		stream >> obj;
    }
    
	archivo.close();

return hechos;
}



//EXTRAER ANTECEDENTES
//Dada una regla, extrae los hechos de su antecedente
vector<string> extraerAntecedentes(Regla regla){
    vector<string> antecedentes;
    regla.antecedente = regla.antecedente.substr(4);
    istringstream stream(regla.antecedente);
    string token;
    while (stream >> token) {
        
        if (token != "Si" && token != "y" && token != "o") {
            // Agregar el literal a la lista
            antecedentes.push_back(token);
        }
    }
	    
	for (size_t i = 0; i < antecedentes.size(); ++i) {
	    	const string &ant = antecedentes[i];
	    }

return antecedentes;
}


//Comprueba si la meta esta en la Base de Hechos
int isMetaInBH(vector<Hecho> hechos, string meta){
	for (size_t i = 0; i < hechos.size(); ++i) {
    	const Hecho &hecho = hechos[i];
           if(hecho.hecho==meta){
                return 1;
        	}
		}
		
	return 0;
}




//EQUIPARAR
//Devuelve un vector con las reglas a traves de las cuales se llega a la meta
vector<Regla> equiparar(vector<Regla> reglas, string meta){
	vector<Regla> CC;

	for (size_t i = 0; i < reglas.size(); ++i) {
                    const Regla &regla = reglas[i];                    
                    if(regla.consecuente==meta){
						CC.push_back(regla);
                   }
                }
    return CC;
}



//Dada una regla conjuntiva calcula el FC (Caso 1 "Y")
double resolverReglaY(Regla regla, vector<Hecho> hechos){
	int cont = 0, dos;
    double primero, segundo;
    primero = segundo = dos = 0;
    
    for (size_t i = 0; i < hechos.size(); ++i) {
    	const Hecho &hecho = hechos[i];
           	/*si encontramos el primer hecho lo guardamos y aumentamos el contador
			para hacer saber que si volvemos a hacer cierta la condicion se debe guardar el 
			nuevo valor en otra variable*/
    		if(regla.antecedente.find(hecho.hecho)!=std::string::npos){
    			if(cont==0){
    				primero = hecho.factorCerteza;
				}else if(cont==1){
					segundo = hecho.factorCerteza;
					//esta variable sirve para saber si estan los dos hechos presentes
					dos=1;
				}
		cont++;
		}
	}
	
	if(dos==1){	
		return regla.factorCerteza * std::min(primero, segundo);
	}else{
		return 0;
	}
	
}

//Dada una regla comprueba si es conjuntiva (hA y hB)
int esReglaY(Regla regla){
	istringstream stream(regla.antecedente);
    string token;
    while(stream >> token){
    	if(token=="y"){
    		return 1;
		}
	}
	return 0;
}


//Dada una regla disyuntiva calcula el FC (Caso 1 "O")
double resolverReglaO(Regla regla, vector<Hecho> hechos){
	int cont = 0, dos;
    double primero, segundo;
    primero = segundo = dos = 0;

    for (size_t i = 0; i < hechos.size(); ++i) {
    	const Hecho &hecho = hechos[i];           
    		if(regla.antecedente.find(hecho.hecho)!=std::string::npos){
    			if(cont==0){
					primero = hecho.factorCerteza;			
				}else if(cont==1){
					segundo = hecho.factorCerteza;
					dos=1;
				}
		cont++;
		}
	}
	
if(dos==1){
	return regla.factorCerteza * std::max(primero, segundo);
}else{
	return regla.factorCerteza * primero;
}
	
}


//Dada una regla comprueba si es disyuntiva (hA o hB)
int esReglaO(Regla regla){
	istringstream stream(regla.antecedente);
    string token;
    while(stream >> token){
    	if(token=="o"){
    		return 1;
		}
	}
	return 0;
}



//Dada una regla de un solo hecho en su antecedente calcula el FC
double resolverReglaU(Regla regla, vector<Hecho> hechos){
	
    double fc=0;
	string aux = regla.antecedente.erase(0, 6);
    for (size_t i = 0; i < hechos.size(); ++i) {
    	const Hecho &hecho = hechos[i];
              		
    		if(aux.find(hecho.hecho)!=std::string::npos){
    		
    			fc=hecho.factorCerteza;
			}
		}

	return regla.factorCerteza * fc;
}



//Dada una regla comprueba si es unitaria (hA -> hX)
int esReglaUnitaria(Regla regla){
	if(esReglaO(regla)==0 && esReglaY(regla)==0){
		return 1;
	}
	
	return 0;
}



//RESOLVER CC
//En base al tipo de regla hace los calculos de los casos 1 y 3, y da formato a la salida
double resolver(Regla regla, vector<Hecho> hechos, ofstream& out){
	double fc;//factor de certeza resultante

    out<<"  Aplicamos: "<<regla.antecedente<<endl;
    if(esReglaO(regla)==1){
    	double r = resolverReglaO(regla, hechos);
    	vector<string> atcs = extraerAntecedentes(regla);
    	out<<"\tCaso 1: "<<atcs.at(0)<<" v "<<atcs.at(1)<<", FC="<<r<<endl<<endl;
		fc=r; 	
	}else if(esReglaY(regla)==1){
		double r = resolverReglaY(regla, hechos);
		vector<string> atcs = extraerAntecedentes(regla);
    	out<<"\tCaso 1: "<<atcs.at(0)<<" y "<<atcs.at(1)<<", FC="<<r<<endl<<endl;
    	fc=r;
	}else if(esReglaUnitaria(regla)==1){
		double r = resolverReglaU(regla, hechos);
		vector<string> atcs = extraerAntecedentes(regla);
		out<<"\tCaso 3: "<<atcs.at(0)<<", FC="<<r<<endl<<endl;
		fc=r;
    }
                                 
return fc;
 
}



//Esta funcion hace los calculos del caso 2
double casoDos(vector<double> &fcs){
	int numReglas = fcs.size();
	
	if(numReglas==0){
		return 0;
	}else if(numReglas==1){
		return fcs.at(0);
	}else if(numReglas==2){
		if(fcs.at(0) >= 0 && fcs.at(1)>=0){
			return fcs.at(0) + fcs.at(1)*(1-fcs.at(0));
		}else if(fcs.at(0) < 0 && fcs.at(0) < 0){
			return fcs.at(0) + fcs.at(1)*(1-fcs.at(0));
		}else if((fcs.at(0) >=0 && fcs.at(1) < 0) || (fcs.at(0) <=0 && fcs.at(1) > 0)){
			return (fcs.at(0)+fcs.at(1))/(1-min(abs(fcs.at(0)), abs(fcs.at(1))));
		}
	}else{
		/*en esta parte se hace el caso 2 de los primeros 2 elmtos del vector. Luego
		se borran y se añade el resultado, para posteriormente hacer el caso 2 de nuevo de 
		el elmto restante con el resultado de los 2 primeros*/
		double r;
		if(fcs.at(0) >= 0 && fcs.at(1)>=0){
			r = fcs.at(0) + fcs.at(1)*(1-fcs.at(0));
		}else if(fcs.at(0) < 0 && fcs.at(0) < 0){
			r = fcs.at(0) + fcs.at(1)*(1-fcs.at(0));
		}else if((fcs.at(0) >=0 && fcs.at(1) < 0) || (fcs.at(0) <=0 && fcs.at(1) > 0)){
			r = (fcs.at(0)+fcs.at(1))/(1-min(abs(fcs.at(0)), abs(fcs.at(1))));
		}
		
		fcs.erase(fcs.begin());
		fcs.erase(fcs.begin());
		fcs.push_back(r);
		casoDos(fcs);
	}	
}

//Dado un hecho (hX) devuelve el FC asociado al mismo
double recuperaFC(vector<Hecho> hechos, string obj){
		for (size_t i = 0; i < hechos.size(); ++i) {
	    const Hecho &hecho = hechos[i];
	   		if(hecho.hecho == obj){
	   			return hecho.factorCerteza;
			}
	}
}



//VERIFICAR
//Comprueba si el objetivo es alcanzable a partir de la BH
int verificar(vector<Regla> reglas, vector<Hecho> &hechos, string meta, double &fc, ofstream& out){
	
int verificado = 0;
vector<Regla> CC; //Conjunto conflicto
vector<string> nuevasMetas;
Regla R;
vector<double> nums; //guarda los FCs resultantes
int check;



if(isMetaInBH(hechos, meta)){//si el objetivo esta en la BH devolver true
	fc = recuperaFC(hechos, meta);
	return 1;	
}else{
	CC = equiparar(reglas, meta);
	if(CC.size()==2){
		check=1;
	}else if(CC.size()==3){
		check=2;
	}

	while(!CC.empty()){

		R = CC.at(0); //seleccionamos la primera regla del CC
		
		out<<R.antecedente<<" (regla activada)"<<endl<<endl;
			
		CC.erase(CC.begin());// eliminamos la regla del CC
		
		nuevasMetas = extraerAntecedentes(R);

		verificado=1;
		
		/*para cada hecho del antecedente se comprueba si esta en la BH
		o se puede alcanzar mediante el uso de alguna otra regla*/
		while(!nuevasMetas.empty() && verificado==1){
			
			string nmet = nuevasMetas.at(0);
			
			nuevasMetas.erase(nuevasMetas.begin());
			
			verificado=verificar(reglas, hechos, nmet, fc, out);
		}

		double s = resolver(R, hechos, out);

		if(s!=0){
			if(check==1 || check == 2){
			nums.push_back(s);
			fc=s;
		}else{
			fc=s;
		}
	} 
	
	
	if(nums.size()>2 && verificado==1){
		fc = casoDos(nums);
		Hecho h = {meta, fc};
		hechos.push_back(h);
		out<<"Caso 2: "<<meta<<", FC="<<fc<<endl<<endl;
		check=0;
	}else if(nums.size()>1 && verificado==1){
		fc = casoDos(nums);
		Hecho h = {meta, fc};
		hechos.push_back(h);
		out<<"Caso 2: "<<meta<<", FC="<<fc<<endl<<endl;
		nums.erase(nums.begin());
		nums.erase(nums.begin());
		nums.push_back(fc);
	}else if(verificado==1 ){

			Hecho h = {meta, s};
			hechos.push_back(h);
			
		}
	}
	
	return verificado;
}

}


int main(int argc, char *argv[]){
	
//Comprueba que se hayan introducido los argumentos necesarios
	if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <BC> <BH>" << std::endl;
        return 1;
    }
    

//VARIABLES    

    const char* BC = argv[1]; //nombre de la BC
    const char* BH = argv[2]; //nombre de la BH
    
    // Formar nombre del fichero de salida
    string s1(BC);
    string s2(BH);
    string tipo = "salida_";
    string sGuion = "_";
    string extension = ".txt";
    s1.erase(s1.length()-4, 4);
    s2.erase(s2.length()-4, 4);
    string res= tipo + s1 + sGuion + s2 + extension;
    const char* resCstr = res.c_str();
    
    const char* nombre = resCstr;
	ofstream out(nombre);
    
	vector<Regla> reglas;
	vector<Hecho> hechos;
	string objetivo;
	reglas = lecturaBC(BC);
	hechos = lecturaBH(BH, objetivo);


//CABECERA

	out<<BC<<endl<<BH<<endl<<"Objetivo: "<<objetivo<<endl<<endl;
	out<<"Reglas:\n";
	
	for (size_t i = 0; i < reglas.size(); ++i) {
	    const Regla &regla = reglas[i];
	    out << regla.antecedente <<" Entonces "<< regla.consecuente<< ", FC: "<< regla.factorCerteza
	              << "\n";
	}
	
	out<<"\nHechos:\n";
	for (size_t i = 0; i < hechos.size(); ++i) {
	    const Hecho &hecho = hechos[i];
	    out <<hecho.hecho <<", FC="<< hecho.factorCerteza<< "\n";
	}


//PROCESO
	double sol = 0;//guarda el resultado final
	out<<endl<<"PROCESO\n";
	verificar(reglas, hechos, objetivo, sol, out);
	out<<"Objetivo: "<<objetivo<<", FC= "<<sol<<endl;
	out.close();
	
	cout<<"Archivo log generado\n";

	
	return 0;

}
