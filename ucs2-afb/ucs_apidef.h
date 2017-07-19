
static const char _afb_description_v2_UNICENS[] =
    "{\"openapi\":\"3.0.0\",\"$schema\":\"http:iot.bzh/download/openapi/schem"
    "a-3.0/default-schema.json\",\"info\":{\"description\":\"\",\"title\":\"u"
    "cs2\",\"version\":\"1.0\",\"x-binding-c-generator\":{\"api\":\"UNICENS\""
    ",\"version\":2,\"prefix\":\"Ucs2\",\"postfix\":\"API\",\"start\":null,\""
    "onevent\":null,\"init\":null,\"scope\":\"\",\"private\":false}},\"server"
    "s\":[{\"url\":\"ws://{host}:{port}/api/monitor\",\"description\":\"Unice"
    "ns2 API.\",\"variables\":{\"host\":{\"default\":\"localhost\"},\"port\":"
    "{\"default\":\"1234\"}},\"x-afb-events\":[{\"$ref\":\"#/components/schem"
    "as/afb-event\"}]}],\"components\":{\"schemas\":{\"afb-reply\":{\"$ref\":"
    "\"#/components/schemas/afb-reply-v2\"},\"afb-event\":{\"$ref\":\"#/compo"
    "nents/schemas/afb-event-v2\"},\"afb-reply-v2\":{\"title\":\"Generic resp"
    "onse.\",\"type\":\"object\",\"required\":[\"jtype\",\"request\"],\"prope"
    "rties\":{\"jtype\":{\"type\":\"string\",\"const\":\"afb-reply\"},\"reque"
    "st\":{\"type\":\"object\",\"required\":[\"status\"],\"properties\":{\"st"
    "atus\":{\"type\":\"string\"},\"info\":{\"type\":\"string\"},\"token\":{\""
    "type\":\"string\"},\"uuid\":{\"type\":\"string\"},\"reqid\":{\"type\":\""
    "string\"}}},\"response\":{\"type\":\"object\"}}},\"afb-event-v2\":{\"typ"
    "e\":\"object\",\"required\":[\"jtype\",\"event\"],\"properties\":{\"jtyp"
    "e\":{\"type\":\"string\",\"const\":\"afb-event\"},\"event\":{\"type\":\""
    "string\"},\"data\":{\"type\":\"object\"}}}},\"x-permissions\":{\"config\""
    ":{\"permission\":\"urn:AGL:permission:UNICENS:public:initialise\"},\"mon"
    "itor\":{\"permission\":\"urn:AGL:permission:UNICENS:public:monitor\"}},\""
    "responses\":{\"200\":{\"description\":\"A complex object array response\""
    ",\"content\":{\"application/json\":{\"schema\":{\"$ref\":\"#/components/"
    "schemas/afb-reply\"}}}}}},\"paths\":{\"/initialise\":{\"description\":\""
    "Initialise Unicens2 lib from NetworkConfig.XML.\",\"get\":{\"x-permissio"
    "ns\":{\"$ref\":\"#/components/x-permissions/config\"},\"parameters\":[{\""
    "in\":\"query\",\"name\":\"filename\",\"required\":true,\"schema\":{\"typ"
    "e\":\"string\"}}],\"responses\":{\"200\":{\"$ref\":\"#/components/respon"
    "ses/200\"}}}},\"/volume\":{\"description\":\"Set Master Volume.\",\"get\""
    ":{\"x-permissions\":{\"$ref\":\"#/components/x-permissions/monitor\"},\""
    "parameters\":[{\"in\":\"query\",\"name\":\"value\",\"required\":true,\"s"
    "chema\":{\"type\":\"integer\"}}],\"responses\":{\"200\":{\"$ref\":\"#/co"
    "mponents/responses/200\"}}}},\"/monitor\":{\"description\":\"Subscribe t"
    "o Unicens Event.\",\"get\":{\"x-permissions\":{\"$ref\":\"#/components/x"
    "-permissions/monitor\"},\"responses\":{\"200\":{\"$ref\":\"#/components/"
    "responses/200\"}}}}}}"
;

static const struct afb_auth _afb_auths_v2_UNICENS[] = {
	{ .type = afb_auth_Permission, .text = "urn:AGL:permission:UNICENS:public:initialise" },
	{ .type = afb_auth_Permission, .text = "urn:AGL:permission:UNICENS:public:monitor" }
};

 void Ucs2initialiseAPI(struct afb_req req);
 void Ucs2volumeAPI(struct afb_req req);
 void Ucs2monitorAPI(struct afb_req req);

static const struct afb_verb_v2 _afb_verbs_v2_UNICENS[] = {
    {
        .verb = "initialise",
        .callback = Ucs2initialiseAPI,
        .auth = &_afb_auths_v2_UNICENS[0],
        .info = NULL,
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "volume",
        .callback = Ucs2volumeAPI,
        .auth = &_afb_auths_v2_UNICENS[1],
        .info = NULL,
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "monitor",
        .callback = Ucs2monitorAPI,
        .auth = &_afb_auths_v2_UNICENS[1],
        .info = NULL,
        .session = AFB_SESSION_NONE_V2
    },
    { .verb = NULL }
};

const struct afb_binding_v2 afbBindingV2 = {
    .api = "UNICENS",
    .specification = _afb_description_v2_UNICENS,
    .info = NULL,
    .verbs = _afb_verbs_v2_UNICENS,
    .preinit = NULL,
    .init = NULL,
    .onevent = NULL,
    .noconcurrency = 0
};

