"use strict";

function defNamespace(ns) {
	var parts = ns.split(".");
	var path = "";
	var parent = global;
	for (let p of parts) {
		path += "." + p;
		if (p in parent == false) {
			log("Create namespace " + path);
			parent[p] = {};
		}
		parent = parent[p];
	}
	return parent;
}

function defClass(namespace, name, constructor, methods) {

	var ns = defNamespace(namespace);

	// create the proxy constructor if it doesn't exist
	var construct;
	if (name in ns == false) {
		var constrName = namespace.replace(/\./g, "_") + "_" + name;
		construct = (new Function(
			"return function " + constrName + "() {"+
			"	" + constrName + ".impl.apply(this, arguments);"+
			"};"
		))();
		construct.prototype = {
			constructor: construct
		};
		ns[name] = construct;
	} else {
		construct = ns[name];
	}
	// link up the correct constructor implementation
	construct.impl = constructor;

	//construct.prototype.impl_constructor = constructor;
	//construct.prototype.impl_methods = methods;

	// add/update methods
	for (let m in methods) {
		// create the proxy method if it doesn't exist
		if (m in construct.prototype == false) {
			construct.prototype[m] = (new Function(
				"return function " + m + "() {"+
				"	return " + m + ".impl.apply(this, arguments);"+
				"};"
			))();
		}
		// link up the correct method implementation
		construct.prototype[m].impl = methods[m];
	}

	// remove methods
	for (let m in construct.prototype) {
		var v = construct.prototype[m];
		if (v == methods || v == constructor || v == construct) continue;
		if (m in methods == false) {
			methods[m].impl = function() {
				throw new Error("Calling removed method " + m);
			};
		}
	}

}

runScript("../bin/gen/glconstants.js");

function tmp_f5pressed() {
	global.gameBase = "../../tankvolution/hotrodgame";
	runScript(global.gameBase + "/game.js");
}
tmp_f5pressed();

initGraphics();
try {
	run();
} catch (e) {
	log("Error in run(): " + e.stack);
}
destroyGraphics();
