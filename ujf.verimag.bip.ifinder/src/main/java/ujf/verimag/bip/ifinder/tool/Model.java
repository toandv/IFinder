package ujf.verimag.bip.ifinder.tool;

import java.io.File;

import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import java.net.URL;
import java.net.MalformedURLException;

import ujf.verimag.bip.parser.loader.*;
import ujf.verimag.bip.instantiator.*;

import bip2.ujf.verimag.bip.packaging.BipPackage;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.component.*;
import bip2.ujf.verimag.bip.instance.*;

/*
 *
 *
 */

public class Model {

    // members
    private Tool m_tool = null;
    private PackageLoader m_loader = null;
    private Instantiator m_instantiator = null;

    private BipPackage m_package = null;
    
    // constructor
    public Model(Tool tool) {
	m_tool = tool;
	initializeLoader();
	m_instantiator = new Instantiator(m_loader);
    }

    // load package
    public void loadPackage(String x_package) {
	assert(m_loader != null);
	
	m_tool.information("load package '" + x_package + "' ... ", 1);
	
	try {
	    m_package = m_loader.getPackage(x_package, true, false);
	    // n.a. - the package is not validated (3rd arg is false)
	}
	catch (LoadPackageException e) {
	    m_tool.information("ko", 2);
	    m_tool.error(e.getMessage() != null ?
			 e.getMessage() : "load package exception");
	    e.printStackTrace();
	    m_tool.abort();
	}
	
 	m_tool.information("ok", 2);
   }

    // locate atom type in package
    public AtomType locateAtomType(String x_atom) {
	assert(m_package != null);
	
	AtomType type = null;

	m_tool.information("locate atom type '" + x_atom + "' ... ", 1);
	
	for(AtomType at : m_package.getAtomTypes())
	    if (at.getName().equals(x_atom)) { type = at; break; }
	
	if (type == null) {
	    m_tool.information("ko", 2);
	    m_tool.error("atom type '" + x_atom + "' not found");
	    m_tool.abort();
	}

	m_tool.information("ok", 2);

	return type;
    }

    // locate compound type in package
    public CompoundType locateCompoundType(String x_compound) {
	assert(m_package != null);
	
	CompoundType type = null;

	m_tool.information("locate compound type '" + x_compound + "' ... ", 1);

	for(CompoundType ct :  m_package.getCompoundTypes())
	    if (ct.getName().equals(x_compound)) { type = ct; break; }
	
	if (type == null) {
	    m_tool.information("ko", 2);
	    m_tool.error("compound type '" + x_compound + "' not found");
	    m_tool.abort();
	}
	
	m_tool.information("ok", 2);

	return type;
    }
    
    // locate component type in package
    public ComponentType locateComponentType(String x_component) {
	assert(m_package != null);
	
	ComponentType type = null;
	
	m_tool.information("locate component type '" + x_component + "' ... ", 1);

	if (type == null)
	    for(AtomType at : m_package.getAtomTypes())
		if (at.getName().equals(x_component)) { type = at; break; }
	if (type == null)
	    for(CompoundType ct :  m_package.getCompoundTypes())
		if (ct.getName().equals(x_component)) { type = ct; break; }
	
	if (type == null) {
	    m_tool.information("ko", 2);
	    m_tool.error("component type '" + x_component + "' not found");
	    m_tool.abort();
	}
	m_tool.information("ok", 2);

	return type;
    }
   
    // create an instance of some give type (e.g., the root)
    public ComponentInstance createComponentInstance(ComponentType type, String name) {
	assert(m_package != null);

	ComponentInstance instance = null;
	
	m_tool.information("instantiate component type '" + type.getName() + "' ... ", 1);
	
	// prepare component instance declaration
	StringBuffer context =
	    new StringBuffer(type.getName() + " " + name + "()");
	ComponentDeclaration compDecl =
	    ComponentFactory.eINSTANCE.createComponentDeclaration();
	compDecl.setLocationInfo(1, 0);
	compDecl.setType(type);
	compDecl.setName(name);
	compDecl.setSourceStringBuffer(context);
	
	// create using the instantiator
	try {
	    instance = m_instantiator.instantiateTopLevel(compDecl, true);
	}
	catch (InstantiatorException e) {
	    m_tool.information("ko", 2);
	    m_tool.error(e.getMessage() != null ?
			 e.getMessage() : "instantiator exception");
	    e.printStackTrace();
	    m_tool.abort();
	}

	m_tool.information("ok", 2);

	return instance;
    }

    
    
    // initialize package loader
    private void initializeLoader() {
	
	// compute the URL of the working directory
	String userDirPath = System.getProperty("user.dir");
	File userDir = new File(userDirPath);
	URL userDirURL = null; 
	
	try {
	    userDirURL = userDir.toURI().toURL();
	}
	catch (MalformedURLException e) {
	    m_tool.error(e.getMessage() != null ?
			 e.getMessage() : "malformed URL exception");
	    e.printStackTrace();
	    m_tool.abort();
	}
	
	// create the BIP package loader 
	URL urls[] = { userDirURL };
	m_loader = new PackageLoader(urls);
    }
    
}
