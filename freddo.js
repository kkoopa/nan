#!/usr/bin/env node
/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

var commander = require('commander'),
    fs = require('fs'),
    glob = require('glob'),
    callbacks = [
      'NAN_METHOD',
      'NAN_GETTER',
      'NAN_SETTER',
      'NAN_PROPERTY_GETTER',
      'NAN_PROPERTY_SETTER',
      'NAN_PROPERTY_ENUMERATOR',
      'NAN_PROPERTY_DELETER',
      'NAN_PROPERTY_QUERY',
      'NAN_INDEX_GETTER',
      'NAN_INDEX_SETTER',
      'NAN_INDEX_ENUMERATOR',
      'NAN_INDEX_DELETER',
      'NAN_INDEX_QUERY'],
    removed = [
      'GetIndexedPropertiesExternalArrayDataLength',
      'GetIndexedPropertiesExternalArrayData',
      'GetIndexedPropertiesExternalArrayDataType',
      'GetIndexedPropertiesPixelData',
      'GetIndexedPropertiesPixelDataLength',
      'HasIndexedPropertiesInExternalArrayData',
      'HasIndexedPropertiesInPixelData',
      'SetIndexedPropertiesToExternalArrayData',
      'SetIndexedPropertiesToPixelData'],
    toconverters = [
      'Boolean',
      'Number',
      'String',
      'Object',
      'Integer',
      'Uint32',
      'Int32'],
    tovalues = [
      ['bool', 'Boolean'],
      ['double', 'Number'],
      ['int64_t', 'Integer'],
      ['uint32_t', 'Uint32'],
      ['int32_t', 'Int32']],
    patterns = [],
    length,
    i;

fs.readFile('package.json', 'utf8', function (err, data) {
  if (err) {
    throw err;
  }

  commander
      .version(JSON.parse(data).version)
      .usage('[options] <file ...>')
      .parse(process.argv);

  if (!process.argv.slice(2).length) {
    commander.outputHelp();
  }
});

patterns = patterns.concat([
  [0, '_NAN_'],
  [0, 'NODE_SET_METHOD'],
  [0, 'NODE_SET_PROTOTYPE_METHOD']]);

patterns = patterns.concat([
  [0, 'NanAsciiString'],
  [0, 'NanEscapeScope'],
  [0, 'NanReturnNull'],
  [0, 'NanReturnValue'],
  [0, 'NanUcs2String']]);

for (i = 0, length = callbacks.length; i < length; i++) {
  patterns.push([1,
      '(' + callbacks[i] + '\\([^\\)]*\\)\\s*\\{)\\s*NanScope\\(\\)\\s*;']);
}

for (i = 0, length = removed.length; i < length; i++) {
  patterns.push([0, ['^.*?(', removed[i], ')'].join('')]);
}

for (i = 0, length = toconverters.length; i < length; i++) {
  patterns.push([1, '([^\\s-]+)->To' + toconverters[i] + '\\(']);
}

for (i = 0, length = tovalues.length; i < length; i++) {
  patterns.push([1, '([^\\s-]+)->(' + tovalues[i][1] + 'Value)\\(']);
}

patterns = patterns.concat([
  [1, '(NanEscapableScope)\\(\\)'],
  [1, '(NanReturnUndefined)\\(\\)'],
  [1, '(NanScope)\\(\\)'],
  [1, '(?:v8\\:\\:)?(TryCatch)'],
  [2, '(NAN_WEAK_CALLBACK)\\(([^\\s\\)]+)\\)'],
  [2, '(NanDisposePersistent)\\s*\\(\\s*([^\\s\\)]+)'],
  [2, '(NanObjectWrapHandle)\\s*\\(([^\s\\)]+)'],
  [2, '(NanSetWeak)\\s*\\(\\s*([^\\s,]+)\\s*,'],
  [2, '([^\\s-]+)->(CallAsConstructor)\\('],
  [2, '([^\\s-]+)->(CallAsFunction)\\('],  
  [2, '([^\\s-]+)->(CloneElementAt)\\('],
  [2, '([^\\s-]+)->(Delete)\\('],
  [2, '([^\\s-]+)->(Equals)\\(([^\\s\\)]+)'],
  [2, '([^\\s-]+)->(ForceSet)\\('],
  [2, '([^\\s-]+)->(Get)\\('],
  [2, '([^\\s-]+)->(GetEndColumn)\\('],
  [2, '([^\\s-]+)->(GetFunction)\\('],
  [2, '([^\\s-]+)->(GetLineNumber)\\('],
  [2, '([^\\s-]+)->(GetOwnPropertyNames)\\('],
  [2, '([^\\s-]+)->(GetPropertyAttributes)\\('],
  [2, '([^\\s-]+)->(GetPropertyNames)\\('],
  [2, '([^\\s-]+)->(GetRealNamedProperty)\\('],
  [2, '([^\\s-]+)->(GetRealNamedPropertyInPrototypeChain)\\('],
  [2, '([^\\s-]+)->(GetSourceLine)\\('],
  [2, '([^\\s-]+)->(GetStartColumn)\\('],
  [2, '([^\\s-]+)->(Has)\\('],
  [2, '([^\\s-]+)->(HasOwnProperty)\\('],
  [2, '([^\\s-]+)->(HasRealIndexedProperty)\\('],
  [2, '([^\\s-]+)->(HasRealNamedCallbackProperty)\\('],
  [2, '([^\\s-]+)->(HasRealNamedProperty)\\('],
  [2, '([^\\s-]+)->(NewInstance)\\('],
  [2, '([^\\s-]+)->(ObjectProtoToString)\\('],
  [2, '([^\\s-]+)->(Set)\\('],
  [2, '([^\\s-]+)->(SetPrototype)\\('],
  [2, '([^\\s-]+)->(StackTrace)\\(\\)'],
  [2, '(\\S+)->(ToArrayIndex)\\('],
  [2, '(\\S+)->(ToDetailString)\\('],
  [3, '(NanAssignPersistent)(?:<v8\\:\\:[^>]+>)?\\(([^,]+),\\s*([^\\s\\)]+)\\)'],
  [3, '(\\W)(args)(\\W)'],
  [3, 'NanNew(<(?:v8\\:\\:)?(Date)>)\\([^\\s\\)]*\\)'],
  [3, 'NanNew(<(?:v8\\:\\:)?(String)>)?\\("[^\\"]*"\\)'],
  [3, 'NanNew(<(?:v8\\:\\:)?(RegExp)>)\\([^\\s\\)]*\\)'],
  [3, '(\\W)(?:node\\:\\:)?(ObjectWrap)(\\W)'],
  [3, '(\\W)(?:v8\\:\\:)?(Persistent)(\\W)']]);

for (i = 1, length = patterns.length; i < length; i++) {
  patterns[i][0] += patterns[i - 1][0];
}

console.log(patterns);

master = new RegExp(patterns.map(function (a) { return a[1]; }).join('|'));

function replace(file, s) {
	s = s.replace(master, function (match) {
		
		if (arguments[patterns[0][0]] === '_NAN_')
		  return 'NAN_';
		if (arguments[patterns[1][0]] === 'NODE_SET_METHOD')
		  return 'NanSetMethod';
		if (arguments[patterns[2][0]] ===  'NODE_SET_PROTOTYPE_METHOD')
		  return 'NanSetPrototypeMethod';
		if (arguments[patterns[3][0]] ===  'NanAsciiString')
		  return 'NanUtf8String';
		if (arguments[patterns[4][0]] ===  'NanEscapeScope')
		  return 'scope.Escape';
		if (arguments[patterns[5][0]] ===  'NanReturnNull')
		  return 'info.GetReturnValue().SetNull';
		if (arguments[patterns[6][0]] ===  'NanReturnValue')
		  return 'info.GetReturnValue().Set';
		if (arguments[patterns[7][0]] ===  'NanUcs2String')
		  return 'v8::String::Value';
		
		return match;
		
		/*} else if (p2 === undefined) {  // twos
			switch (p1) {
		        case 'NAN_GETTER':
		        case 'NAN_METHOD':
		        case 'NAN_SETTER':
		        case 'NAN_INDEX_DELETER':
		        case 'NAN_INDEX_ENUMERATOR':
		        case 'NAN_INDEX_GETTER':
		        case 'NAN_INDEX_QUERY':
		        case 'NAN_INDEX_SETTER':
		        case 'NAN_PROPERTY_DELETER':
		        case 'NAN_PROPERTY_ENUMERATOR':
			    case 'NAN_PROPERTY_GETTER':
		        case 'NAN_PROPERTY_QUERY':
		        case 'NAN_PROPERTY_SETTER':
				  return p1;
      		    case 'GetIndexedPropertiesExternalArrayData':
      		    case 'GetIndexedPropertiesExternalArrayDataLength':
      		    case 'GetIndexedPropertiesExternalArrayDataType':
      		    case 'GetIndexedPropertiesPixelData':
      		    case 'GetIndexedPropertiesPixelDataLength':
      		    case 'HasIndexedPropertiesInExternalArrayData':
      		    case 'HasIndexedPropertiesInPixelData':
      		    case 'SetIndexedPropertiesToExternalArrayData':
      		    case 'SetIndexedPropertiesToPixelData':
      		      return '// ERROR: Rewrite using Buffer\n' +  match;
		  	    case 'Boolean':
			    case 'Int32':
			    case 'Integer':
		        case 'Number':
		        case 'Object':
		        case 'String':
		        case 'Uint32':
        		  return ['NanTo<', tovalues[i][0], '>(',  p1].join('');
        		case 'BooleanValue':
        		  return ['NanTo<bool>(',  p1].join('');
        		case 'Int32Value':
        		  return ['NanTo<int32_t>(',  p1].join('');
        		case 'IntegerValue':
        		  return ['NanTo<int64_t>(',  p1].join('');
        		case 'Uint32Value':
        		  return ['NanTo<uint32_t>(',  p1].join('');
				case 'NanEscapableScope':
				  return 'NanEscapableScope scope';
				case 'NanReturnUndefined':
				  return 'return';
				case 'NanScope':
				  return 'NanScope scope';
			    case 'TryCatch':
		          return 'NanTryCatch';
                default:
                  console.error(['impossible match 2: ', match].join(''));
		  	      return match;
			}
		} else if (p3 === undefined) {  // threes
			switch (p1) {
			  case 'NAN_WEAK_CALLBACK':
			    return ['template<typename T>\nvoid ',
                    p1, '(const Nan::NanWeakCallbackInfo<T> &data)'].join('');
			  case 'NanDisposePersistent':
			    return [p1, '.Reset('].join('');
			  case 'NanObjectWrapHandle':
			    return [p1, '->handle('].join('');
			  case 'NanSetWeak':
			    return [p1, '.SetWeak('].join('');
              default:
	            switch (p2) {
			      case 'CallAsConstructor':
			        return ['NanCallAsConstructor(', p1].join('');
			      case 'CallAsFunction':
			        return ['NanCallAsFunction(', p1].join('');
			      case 'CloneElementAt':
			        return ['NanCloneElementAt(', p1].join('');
			      case 'Delete':
			        return ['NanDelete(', p1].join('');
        		  case 'Equals':
        		    return ['NanEquals(', p1, ', ', p2].join('');
			      case 'ForceSet':
			        return ['NanForceSet(', p1].join('');
			      case 'Get':
			        return ['NanGet(', p1].join('');
			      case 'GetEndColumn':
			        return ['NanGetEndColumn(', p1].join('');
			      case 'GetFunction':
			        return ['NanGetFunction(', p1].join('');
			      case 'GetLineNumber':
			        return ['NanGetLineNumber(', p1].join('');
			      case 'GetOwnPropertyNames':
			        return ['NanGetOwnPropertyNames(', p1].join('');
			      case 'GetPropertyAttributes':
			        return ['NanGetPropertyAttributes(', p1].join('');
			      case 'GetPropertyNames':
			        return ['NanGetPropertyNames(', p1].join('');
			      case 'GetRealNamedProperty':
			        return ['NanGetRealNamedProperty(', p1].join('');
			      case 'GetRealNamedPropertyInPrototypeChain':
			        return ['NanGetRealNamedPropertyInPrototypeChain(', p1].join('');
			      case 'GetSourceLine':
			        return ['NanGetSourceLine(', p1].join('');
			      case 'GetStartColumn':
			        return ['NanGetStartColumn(', p1].join('');
			      case 'Has':
			        return ['NanHas(', p1].join('');
			      case 'HasOwnProperty':
			        return ['NanHasOwnProperty(', p1].join('');
			      case 'HasRealIndexedProperty':
			        return ['NanHasRealIndexedProperty(', p1].join('');
			      case 'HasRealNamedCallbackProperty':
			        return ['NanHasRealNamedCallbackProperty(', p1].join('');
			      case 'HasRealNamedProperty':
			        return ['NanHasRealNamedProperty(', p1].join('');
			      case 'NewInstance':
			        return ['NanNewInstance(', p1].join('');
			      case 'ObjectProtoToString':
			        return ['NanObjectProtoToString(', p1].join('');
			      case 'Set':
			        return ['NanSet(', p1].join('');
			      case 'SetPrototype':
			        return ['NanSetPrototype(', p1].join('');
			      case 'StackTrace':
			        return [p1, '.StackTrace().ToLocalChecked'].join('');
			      case 'ToArrayIndex':
			        return ['NanToArrayIndex(', p1].join('');
			      case 'ToDetailString':
			        return ['NanToDetailString(', p1].join('');
			      default:
			        console.error(['impossible match 3: ', match].join(''));
		  	        return match;
				}
			}
			
	    } else {  // fours
	    	if (p1 === 'NanAssignPersistent') {
	    		return [p1, '.Reset(', p2, ')'].join('');
	    	}
	    	console.log('p2: ' + p2);
	    	switch (p2) {
	    		case 'args':
	    		  return [p1, 'info', p3].join('');
	    		case 'ObjectWrap':
	    		  return [p1, 'NanObjectWrap', p2].join('');
	    		case 'Persistent':
	    		  return [p1, 'NanPersistent', p2].join('');
	    		case 'Date':
	    		case 'String':
	    		case 'RegExp':
	    		  return ['NanNew', p1, '(', p2, ').ToLocalChecked()'].join('');
	    		default:
	    		  console.error(['impossible match 4: ', match].join(''));
		  	      return match;
	    	}
		}*/
	});
	fs.writeFile(file, s, function (err) {
      if (err) {
        throw err;
      }
    });
}

function processFile(file) {
  fs.readFile(file, {encoding: 'utf8'}, function (err, data) {
    if (err) {
      throw err;
    }

    replace(file, data);
  });
}

for (i = 2, length = process.argv.length; i < length; i++) {
  glob(process.argv[i], function (err, matches) {
    if (err) {
      throw err;
    }
    matches.forEach(function (currentValue) {
      processFile(currentValue);
    });
  });
}
