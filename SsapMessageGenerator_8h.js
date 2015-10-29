var SsapMessageGenerator_8h =
[
    [ "ssap_message", "structssap__message.html", "structssap__message" ],
    [ "keyValuePair", "structkeyValuePair.html", "structkeyValuePair" ],
    [ "map_t", "structmap__t.html", "structmap__t" ],
    [ "bulkRequest", "structbulkRequest.html", "structbulkRequest" ],
    [ "SSAPMSGGEN_API", "SsapMessageGenerator_8h.html#a06d6ed3aed01853214d1ef8b9e0c0a3f", null ],
    [ "SSAPMessageDirection", "SsapMessageGenerator_8h.html#aeeedba07e0378132c7d542801a585abf", [
      [ "REQUEST", "SsapMessageGenerator_8h.html#aeeedba07e0378132c7d542801a585abfabaf6b8e08edf0e9d2eb38551fed1fb39", null ],
      [ "RESPONSE", "SsapMessageGenerator_8h.html#aeeedba07e0378132c7d542801a585abfafb95ee545d749f11305939b93fe2bda3", null ],
      [ "ERROR", "SsapMessageGenerator_8h.html#aeeedba07e0378132c7d542801a585abfa2fd6f336d08340583bd620a7f5694c90", null ]
    ] ],
    [ "SSAPMessageTypes", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664", [
      [ "JOIN", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a4925a399dab94b9b58f6d1b5cd246af7", null ],
      [ "LEAVE", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664ae09e07839103de682cb13fa773793fc0", null ],
      [ "INSERT", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664aa15c451953b2d2a93403afe786930d0f", null ],
      [ "UPDATE", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a3912ed627c0090ccc7fa1b03fef04202", null ],
      [ "DELETE", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a9d61e82a9a12752f10aece1b22183913", null ],
      [ "QUERY", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a21043ddfa5289b4cf14cd4e3f5a89b62", null ],
      [ "SUBSCRIBE", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664abc6f919ff681f5f552b2f7d1f0fba832", null ],
      [ "UNSUBSCRIBE", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a8395e5981c15d813c588c86988fd4aea", null ],
      [ "INDICATION", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a30fee833d15d3b72399feb9a84e356cb", null ],
      [ "BULK", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a7cf8c698367d8a7e5f568d30b4b17910", null ],
      [ "CONFIG", "SsapMessageGenerator_8h.html#a936e20f72ed4897a1387c0b95aeba664a702582f7f825ca83bdb076b15b4c0fc2", null ]
    ] ],
    [ "SSAPPersistenceType", "SsapMessageGenerator_8h.html#ad3b71bc481f69e3dbb0fe6f403ec8e85", [
      [ "MONGODB", "SsapMessageGenerator_8h.html#ad3b71bc481f69e3dbb0fe6f403ec8e85a3d27d19671a784f1639d97adf95bcbc3", null ],
      [ "INMEMORY", "SsapMessageGenerator_8h.html#ad3b71bc481f69e3dbb0fe6f403ec8e85afa67de955b5cb874f8acc784f9afbee6", null ]
    ] ],
    [ "SSAPQueryType", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1", [
      [ "NATIVE", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1a26741459537a01adc304c787cdd3243e", null ],
      [ "SQLLIKE", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1a692b7f1f0690d10589cf27920f17d77a", null ],
      [ "SIB_DEFINED", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1a34e00ebb0fc44b4e65b9e168f5f15ae9", null ],
      [ "BDH", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1ab0ad250dd18a7d031bc1ea217a363c78", null ],
      [ "CEP", "SsapMessageGenerator_8h.html#a9c0c158bf7ca3d73a1e9d2bd8f712cd1af706f13ca3bb4538163494a9b167572c", null ]
    ] ],
    [ "addToBulkRequest", "SsapMessageGenerator_8h.html#a9d87675c2756c8bbbd54f02097544469", null ],
    [ "createBulkRequest", "SsapMessageGenerator_8h.html#a7e64b2dc5186c18bb1a2d98e7f9c7a04", null ],
    [ "createEmptyMap", "SsapMessageGenerator_8h.html#a379ab86f85ecb258e4c0e22d6694b27c", null ],
    [ "freeSsapMessage", "SsapMessageGenerator_8h.html#a90280218716f3f442c633d28443dedf1", null ],
    [ "generateBulkMessage", "SsapMessageGenerator_8h.html#a56a979460c5072817fdd25c58e486968", null ],
    [ "generateConfigMessage", "SsapMessageGenerator_8h.html#a6c1588cfec9f17a6a725dc4960d216cd", null ],
    [ "generateDeleteMessage", "SsapMessageGenerator_8h.html#a52e3903c2a87b5d716f885730d7055d9", null ],
    [ "generateDeleteMessageWithQueryType", "SsapMessageGenerator_8h.html#a5644fa236b07d9d9640ef0bbd2555c63", null ],
    [ "generateInsertMessage", "SsapMessageGenerator_8h.html#a7d54ddf0053f11efa09ff2b2142f83ce", null ],
    [ "generateInsertMessageWithQueryType", "SsapMessageGenerator_8h.html#aa3b37d67b3d324bc2d71fe51a3f78477", null ],
    [ "generateJoinMessage", "SsapMessageGenerator_8h.html#a8eb3483515f92b49c69378422cefa0db", null ],
    [ "generateJoinRenewSessionMessage", "SsapMessageGenerator_8h.html#ac85d5aa5c81afb01fb5de23728ec4b56", null ],
    [ "generateLeaveMessage", "SsapMessageGenerator_8h.html#af8c80c301697824ec192cda37843ace2", null ],
    [ "generateQueryMessage", "SsapMessageGenerator_8h.html#a5919e4013492dbd0c03e373d25693e4a", null ],
    [ "generateQueryMessageWithQueryType", "SsapMessageGenerator_8h.html#ae5be0c2371e21f99598f8fcd2d9bbaa4", null ],
    [ "generateSIBDefinedQueryMessageWithParams", "SsapMessageGenerator_8h.html#a224d13a88cd8d691c2b88538a9b750c0", null ],
    [ "generateSubscribeMessage", "SsapMessageGenerator_8h.html#ae2ce08c5401cb3f880c052f80c3b9a65", null ],
    [ "generateSubscribeMessageWithQueryType", "SsapMessageGenerator_8h.html#aa5cb7b6a2a7400b2a4810c6fdb45ac0e", null ],
    [ "generateUnsubscribeMessage", "SsapMessageGenerator_8h.html#a741269421e8a91a1e0976b6d9cafae0e", null ],
    [ "generateUpdateMessage", "SsapMessageGenerator_8h.html#a6662b541527958181959133db48ca496", null ],
    [ "generateUpdateMessageWithQueryType", "SsapMessageGenerator_8h.html#afef75cf1be75993b2c410109becc4749", null ],
    [ "pushKeyValuePair", "SsapMessageGenerator_8h.html#a567a66bb3c208c36eee1c05f917f46ad", null ],
    [ "ssap_messageToJson", "SsapMessageGenerator_8h.html#a21872bcb8eb316da35a44d3fb7afb8de", null ],
    [ "ssapMessageFromJson", "SsapMessageGenerator_8h.html#a7a598386aaf412d34c7b1c14ec67eb12", null ]
];