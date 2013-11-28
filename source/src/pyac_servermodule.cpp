#include "cube.h"
#include "pyac_servermodule.h"

extern vector<client *> clients;

static PyObject *py_logline(PyObject *self, PyObject *args)
{
    char *s;
    int level = ACLOG_INFO;
    if(!PyArg_ParseTuple(args,  "s|i", &s, &level))
        return NULL;
    
    if (-1 < level && level < ACLOG_NUM) {
        logline(level,s);
    }
    
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_sendmsg(PyObject *self, PyObject *args) {
    char *msg;
    int cn = -1;
    if(!PyArg_ParseTuple(args,  "s|i", &msg,&cn))
        return NULL;
    
    sendservmsg(msg,cn);
    return Py_None;
}

static PyObject *py_getclient(PyObject *self, PyObject *args) {
    int cn;
    client *cl;

    if(!PyArg_ParseTuple(args,  "i", &cn)) return NULL;
    
    if (valid_client(cn)) {
        cl = clients[cn];
    }
    else {
        return Py_None;
    }

    int millis = servmillis - cl->connectmillis;
    //                 //  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21     22 23 24 25 26
    return Py_BuildValue("{si,ss,ss,si,si,si,si,si,si,si,si,si,si,si,si,si,si,si,si,si,s(fff),si,si,si,si,si}",
                         "cn",cl->clientnum,                     // 1
                         "hostname",cl->hostname,                // 2
                         "name",cl->name,                        // 3
                         "ping",cl->ping,                        // 4
                         "role",cl->role,                        // 5
                         "version",cl->acversion,                // 6
                         "buildtype",cl->acbuildtype,            // 7
                         "vote",cl->vote,                        // 8
                         "millis",millis,                        // 9
                         "frags",cl->state.frags,                // 10
                         "flags",cl->state.flagscore,            // 11
                         "scoped",cl->state.scoped?1:0,          // 12
                         "akimbomillis",cl->state.akimbomillis,  // 13
                         "teamkills",cl->state.teamkills,        // 14
                         "shotdamage",cl->state.shotdamage,      // 15
                         "damage",cl->state.damage,              // 16
                         "deaths",cl->state.deaths,              // 17
                         "points",cl->state.points,              // 18
                         "lastshot",cl->state.lastshot,          // 19
                         "state",cl->state.state,                // 20
                         "pos",cl->state.o.x,cl->state.o.x, cl->state.o.y,        // 21
                         "health",cl->state.health,              // 22
                         "armour",cl->state.armour,              // 23
                         "primary",cl->state.primary,            // 24
                         "nextprimary",cl->state.nextprimary,    // 25
                         "gunselect",cl->state.gunselect         // 26
                         );
}

static PyObject *py_killclient(PyObject *self, PyObject *args) {
    int tcn,acn,gib,weap ;
    if(!PyArg_ParseTuple(args,  "ii|i",&acn,&tcn,&gib,&weap ))
    
    if(!valid_client(tcn)) return Py_None;
    if(!valid_client(acn)) return Py_None ;
    
    sendf(-1, 1, "ri5", gib ? SV_GIBDIED : SV_DIED, tcn, acn, clients[tcn]->state.frags, weap);
    
    return Py_None;
}

static PyMethodDef ModuleMethods[] = {
	{"log", py_logline, METH_VARARGS, "Logs a message."},
    {"msg", py_sendmsg, METH_VARARGS, "Sends a server message."},
    {"getClient", py_getclient, METH_VARARGS, "Gets a client dictionary."},
    {"killClient", py_killclient, METH_VARARGS, "Kills a acn as if tcn killed them."},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initModule()
{
	(void) Py_InitModule("acserver", ModuleMethods);
	return;
}