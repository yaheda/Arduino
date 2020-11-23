import 'package:flutter/material.dart';
import 'package:english_words/english_words.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'locations.dart' as locations;
import 'authresponse.dart';
import 'telemetry.dart';

import 'package:http/http.dart' as http;
import 'dart:async';
import 'dart:convert';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Statup Name Generator',
      theme: ThemeData(primaryColor: Colors.orange),
      home: RandomWords()
    );
  }
}

class RandomWords extends StatefulWidget {
  @override
  _RandomWordsState createState() => _RandomWordsState();
}

class _RandomWordsState extends State<RandomWords> {
  final _suggestions = <WordPair>[];
  final _saved = Set<WordPair>();
  final _biggerFont = TextStyle(fontSize: 18);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Startup Name Generator'),
        actions: [
          IconButton(icon: Icon(Icons.list), onPressed: _pushedSaved),
          IconButton(icon: Icon(Icons.add_location), onPressed: _pushedMap)
        ],),
      body: _buildSuggestions(),
    );
  }

  void _pushedMap() {
    Navigator.of(context).push(
      MaterialPageRoute(builder: (BuildContext context) {
        return Scaffold(
          appBar: AppBar(title: Text('Maps'),),
          body: GoogleMap(
            onMapCreated: _onMapCreated,
            initialCameraPosition: CameraPosition(target: const LatLng(0, 0), zoom: 4),
            markers: _markers.values.toSet(),
          ),
        );
      })
    );
  }

  GoogleMapController mapController;
  final LatLng _center = const LatLng(45.521563, -122.677433);

  final Map<String, Marker> _markers = {};

  Future<void>  _onMapCreated(GoogleMapController controller) async{
    final googleOffices = await locations.getGoogleOffices();

    final authenticateResponse = await authenticateToThingsBoard();
    final telemetryResponse = await getLatestTelemetry(authenticateResponse.token);

    setState(() {
      _markers.clear();

      final marker = Marker(
        markerId: MarkerId('Tracker0'),
        position: LatLng(telemetryResponse.latitude, telemetryResponse.longitude),
        infoWindow: InfoWindow(
            title: 'Tracker0',
            snippet: 'hello from cameroon'
        )
      );
      _markers['Tracker0'] = marker;

      /*
      for(final office in googleOffices.offices) {
        final marker = Marker(
          markerId: MarkerId(office.name),
          position: LatLng(office.lat, office.lng),
          infoWindow: InfoWindow(
            title: office.name,
            snippet: office.address
          )
        );
        _markers[office.name] = marker;
      }*/
    });
  }

  //-----

  Future<AuthResponse> authenticateToThingsBoard() async {
    final thingsboard_url = 'demo.thingsboard.io';
    final http.Response response = await http.post(
      'http://$thingsboard_url/api/auth/login',
      headers: <String, String>{
        'Content-Type': 'application/json; charset=UTF-8',
        'Accept': 'application/json',
      },
      body: jsonEncode(<String, String>{
        'username': 'yaheda.a@gmail.com',
        'password': '4b0ub4k4r'
      }),
    );

    if (response.statusCode == 200) {
      return AuthResponse.fromJson(jsonDecode(response.body));
    }

    throw Exception('Failed to authenticate to thingsboard');
  }

  Future<Telemetry> getLatestTelemetry(String jwtToken) async {
    final thingsboard_url = 'demo.thingsboard.io';
    final deviceId = 'ade34a50-f1ff-11ea-bcda-a5152a32a9f6';
    final http.Response response = await http.get(
      'http://$thingsboard_url/api/plugins/telemetry/DEVICE/$deviceId/values/timeseries?keys=latitude,longitude',
      headers: <String, String>{
        'Content-Type': 'application/json; charset=UTF-8',
        'X-Authorization': 'Bearer $jwtToken',
      },
    );

    if (response.statusCode == 200) {
      return Telemetry.fromJson(jsonDecode(response.body));
    }

    final xx = jsonDecode(response.body);

    throw Exception('Failed to get telemetry from thingsboard');
  }





  //-----

  void _pushedSaved() {
    Navigator.of(context).push(
      MaterialPageRoute<void>(
          builder: (BuildContext context){
           final tiles = _saved.map((WordPair pair) => ListTile(title: Text(pair.asPascalCase, style: _biggerFont,),));
           final divided = ListTile.divideTiles(context: context, tiles: tiles).toList();

           return Scaffold(
             appBar: AppBar(title: Text('Saved Suggestions')),
             body: ListView(children: divided,));
          })
    );
  }

  Widget _buildSuggestions() {
    return ListView.builder(
        padding: EdgeInsets.all(16.0),
        itemBuilder: (context, i) {
          if (i.isOdd) return Divider();

          final index = i ~/ 2;
          if (index >= _suggestions.length) {
            _suggestions.addAll(generateWordPairs().take(10));
          }
          return _buildRow(_suggestions[index]);
        }
    );
  }

  Widget _buildRow(WordPair pair) {
    final alreadySaved = _saved.contains(pair);

    return ListTile(
      title: Text(pair.asPascalCase, style: _biggerFont),
      trailing: Icon(
        alreadySaved ? Icons.favorite : Icons.favorite_border,
        color: alreadySaved ? Colors.red : null,

      ),
      onTap: () {
        setState(() {
          if (alreadySaved) {
            _saved.remove(pair);
          } else {
            _saved.add(pair);
          }
        });
      },
    );
  }
}