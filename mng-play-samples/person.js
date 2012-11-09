(function() {
  var AddNode, AddParser, BinaryNode, ConstantNode, ExpressionParser, FFI, MultiplyNode, MultiplyParser, NodeParser, VariableNode, WaveNode, WaveParser, argv, assertin, condition_parser, m, mng, parse, peek, tokens, _;
  var __hasProp = Object.prototype.hasOwnProperty, __extends = function(child, parent) {
    for (var key in parent) { if (__hasProp.call(parent, key)) child[key] = parent[key]; }
    function ctor() { this.constructor = child; }
    ctor.prototype = parent.prototype;
    child.prototype = new ctor;
    child.__super__ = parent.prototype;
    return child;
  };
  FFI = require('node-ffi');
  argv = process.argv.splice(2);
  _ = require('underscore');
  mng = (function() {
    var lib, stdlib;
    lib = new FFI.Library("libmng", {
      mng_file_new_from_filename: ["pointer", ["string"]],
      mng_file_destroy: ["void", ["pointer"]],
      mng_file_get_script: ["string", ["pointer"]],
      mng_lex: ["int", ["string", "pointer"]]
    });
    stdlib = new FFI.Library(null, {
      free: ["void", ["pointer"]]
    });
    return {
      File: function(filename) {
        var pointer;
        pointer = lib.mng_file_new_from_filename(filename);
        this.destroy = function() {
          return lib.mng_file_destroy(pointer);
        };
        return this.__defineGetter__('script', function() {
          return lib.mng_file_get_script(pointer);
        });
      },
      lex: function(script, cb) {
        var success, _cb;
        _cb = new FFI.Callback(["void", ["string", "float", "pointer"]], function(type, lineno, ptr) {
          var data;
          data = null;
          if (type === "word" || type === "comment") {
            data = ptr.getCString();
            stdlib.free(ptr);
          } else if (type === "number") {
            data = ptr.getFloat();
          }
          return cb(type, lineno, data);
        });
        success = lib.mng_lex(script, _cb.getPointer());
        return success !== 0;
      },
      lex_reduce: function(script) {
        var success, tokens;
        tokens = [];
        success = this.lex(script, function(type, lineno, data) {
          return tokens.push([type, lineno, data]);
        });
        if (success) {
          return tokens;
        } else {
          return null;
        }
      }
    };
  })();
  if (argv.length === 0) {
    console.log("Usage: " + process.argv[0] + " " + process.argv[1] + " FILE");
    process.exit(-1);
  }
  m = new mng.File(argv[0]);
  m.destroy();
  peek = function(tokens, desc) {
    var name;
    name = tokens[0];
    return name[0] === "word" && name[2] === desc.name;
  };
  assertin = function(val, list) {
    return list.indexOf(val) !== -1;
  };
  parse = function(tokens, desc) {
    var args, body, name;
    name = tokens.shift();
    if (name[0] !== "word") {
      return false;
    }
    if (name[2] !== desc.name) {
      return false;
    }
    if (tokens.shift()[0] !== "lparen") {
      return false;
    }
    if (desc.args) {
      args = desc.args.map(function(arg_type, index) {
        var tok;
        if (index !== 0) {
          if (tokens.shift()[0] !== "comma") {
            return false;
          }
        }
        tok = tokens.shift();
        if (arg_type === "variable") {
          if (!(tok[0] === "word" || tok[0] === "number")) {
            return false;
          }
        } else {
          if (tok[0] !== arg_type) {
            return false;
          }
        }
        return tok[2];
      });
    } else {
      args = [];
    }
    body = [];
    if (desc.body) {
      if (tokens.shift()[0] !== "lbrace") {
        return false;
      }
      if (tokens.shift()[0] !== "rbrace") {
        return false;
      }
    }
    return {
      args: args,
      body: body
    };
  };
  NodeParser = function(desc) {
    var ret;
    ret = function(tokens) {
      return new desc.klass(parse(tokens, desc));
    };
    ret.peek = function(tokens) {
      return peek(tokens, desc);
    };
    return ret;
  };
  WaveNode = (function() {
    function WaveNode(opts) {
      this.name = opts.args[0];
    }
    WaveNode.prototype.toString = function() {
      return "Wave(" + this.name + ")";
    };
    return WaveNode;
  })();
  WaveParser = NodeParser({
    name: "Wave",
    args: ["word"],
    body: false,
    klass: WaveNode
  });
  tokens = mng.lex_reduce("Wave(Bloop)");
  console.log(tokens);
  console.log(WaveParser(tokens).toString());
  ExpressionParser = function(desc) {
    return NodeParser(_.extend(desc, {
      args: ["variable", "variable"],
      body: false
    }));
  };
  VariableNode = (function() {
    function VariableNode(name) {
      this.name = name;
    }
    VariableNode.prototype.toString = function() {
      return this.name;
    };
    return VariableNode;
  })();
  ConstantNode = (function() {
    function ConstantNode(value) {
      this.value = value;
    }
    ConstantNode.prototype.toString = function() {
      return this.value;
    };
    return ConstantNode;
  })();
  BinaryNode = (function() {
    function BinaryNode(opts) {
      var fixup;
      fixup = function(arg) {
        if (typeof arg === 'string') {
          return new VariableNode(arg);
        } else if (typeof arg === 'number') {
          return new ConstantNode(arg);
        }
      };
      this.left = fixup(opts.args[0]);
      this.right = fixup(opts.args[1]);
    }
    return BinaryNode;
  })();
  MultiplyNode = (function() {
    __extends(MultiplyNode, BinaryNode);
    function MultiplyNode() {
      MultiplyNode.__super__.constructor.apply(this, arguments);
    }
    MultiplyNode.prototype.toString = function() {
      return "Multiply(" + this.left + ", " + this.right + ")";
    };
    return MultiplyNode;
  })();
  MultiplyParser = ExpressionParser({
    name: "Multiply",
    klass: MultiplyNode
  });
  AddNode = (function() {
    __extends(AddNode, BinaryNode);
    function AddNode() {
      AddNode.__super__.constructor.apply(this, arguments);
    }
    AddNode.prototype.toString = function() {
      return "Add(" + this.left + ", " + this.right + ")";
    };
    return AddNode;
  })();
  AddParser = ExpressionParser({
    name: "Add",
    klass: AddNode
  });
  tokens = mng.lex_reduce("Multiply(temp, \n\t0.75)");
  console.log(tokens);
  console.log(MultiplyParser(tokens).toString());
  console.log(AddParser(mng.lex_reduce("Add(5, 6)")).toString());
  condition_parser = function(tokens) {
    var lvalue, name, rvalue, rvalue_desc;
    name = tokens.shift();
    if (name[0] !== "word") {
      return false;
    }
    lvalue = name[2];
    if (tokens.shift()[0] !== "assignment") {
      return false;
    }
    rvalue_desc = _.find([multiply_desc, add_desc], function(desc) {
      return peek(tokens, desc);
    });
    if (!rvalue_desc) {
      return false;
    }
    rvalue = parse(tokens, rvalue_desc);
    return [lvalue, rvalue];
  };
  tokens = mng.lex_reduce("temp = Multiply(temp, 0.75)");
  console.log(condition_parser(tokens));
}).call(this);
