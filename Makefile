CC = g++
CXXFLAGS = -std=c++11 -g
MODULOIO = Archivo.o ArchivoFactory.o Bitacora.o BitacoraFactory.o ArchivoBloque.o ArchivoBloqueFactory.o
UTILS = RegistroBase.o RegistroBaseFactory.o StringUtils.o Bloque.o BloqueFactory.o BloqueRegistro.o BloqueRegistroFactory.o MapaDeBits.o MapaDeBitsFactory.o DescriptorRegistro.o DescriptorRegistroFactory.o TTDispersion.o TTDispersionFactory.o CaracterUtils.o ByteMap.o ByteMapFactory.o
MEMORY = Object.o Pointer.o Feature.o Registro.o
TDA = LinkedList.o LinkedListFactory.o Arbol.o ArbolFactory.o Fecha.o FechaFactory.o
SYSTEM = Sistema.o TraceEntry.o TraceEntryFactory.o StackTrace.o StackTraceFactory.o Entorno.o EntornoFactory.o
EXCEPTIONS = Exception.o ExceptionFactory.o
SERIALIZADORES = SerializadorNumerico.o SerializadorNumericoFactory.o SerializadorCadenaSinPrefijo.o SerializadorCadenaSinPrefijoFactory.o SerializadorByteMap.o SerializadorByteMapFactory.o SerializadorTTDispersion.o SerializadorTTDispersionFactory.o SerializadorCadenaANSI.o SerializadorCadenaANSIFactory.o SerializadorCadenaUNICODE.o SerializadorCadenaUNICODEFactory.o
HIDRATADORES = HidratadorNumerico.o HidratadorNumericoFactory.o HidratadorCadenaSinPrefijo.o HidratadorCadenaSinPrefijoFactory.o HidratadorTTDispersion.o HidratadorTTDispersionFactory.o HidratadorByteMap.o HidratadorByteMapFactory.o HidratadorCadenaANSI.o HidratadorCadenaANSIFactory.o HidratadorCadenaUNICODE.o HidratadorCadenaUNICODEFactory.o
OBJS = $(MODULOIO) $(MEMORY) $(TDA) $(UTILS) $(SERIALIZADORES) $(HIDRATADORES) $(SYSTEM) $(EXCEPTIONS)
APLICACION = Aplicacion.o
CarpetaFuentes =./Fuentes/
OBJDIR = Objects
EXEC = TPDatosGrupal

.PHONY: all
all: clean build

.PHONY: build
build: | $(OBJDIR) $(EXEC)

$(OBJDIR):
	mkdir $(OBJDIR)

.SECONDEXPANSION:
$(MODULOIO): $$(join $(CarpetaFuentes)io/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(MEMORY): $$(join $(CarpetaFuentes)Memoria/, $$(subst .o,, $$@)/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(TDA): $$(join $(CarpetaFuentes)TDA/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(UTILS): $$(join $(CarpetaFuentes)Utils/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(SERIALIZADORES): $$(join $(CarpetaFuentes)Serializadores/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(HIDRATADORES): $$(join $(CarpetaFuentes)Hidratadores/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

.SECONDEXPANSION:
$(SYSTEM): $$(join $(CarpetaFuentes)Sistema/, $$(subst Factory,,$$(subst .o,, $$@))/Fuentes/$$(subst .o,,$$@).cpp)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

$(EXCEPTIONS): %.o: $(CarpetaFuentes)Exceptions/Fuentes/%.cpp
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

$(APLICACION): $(CarpetaFuentes)Aplicacion/Fuentes/Aplicacion.cpp $(OBJS)
	$(CC) $(CXXFLAGS) -c $< -o $(OBJDIR)/$@

$(EXEC): $(CarpetaFuentes)main.cpp $(APLICACION)
	$(CC) $(CXXFLAGS) $(addprefix $(OBJDIR)/, $(OBJS) $(APLICACION)) $< -o $(EXEC)

.PHONY: clean
clean:
	-rm -f Objects/*.o $(EXEC)
