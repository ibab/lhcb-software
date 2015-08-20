#include "Kernel/IConfigTreeEditor.h"

/// Return the interface ID
const InterfaceID& IConfigTreeEditor::interfaceID() { 
      static const InterfaceID id( "IConfigTreeEditor", 3, 0 );
      return id; 
}

IConfigTreeEditor::~IConfigTreeEditor()
{}

