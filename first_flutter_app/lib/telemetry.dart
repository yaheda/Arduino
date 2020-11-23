class Telemetry {
  final double latitude;
  final double longitude;

  Telemetry({this.latitude, this.longitude});

  factory Telemetry.fromJson(Map<String, dynamic> json) {
    final lat = json['latitude'][0]['value'];
    final lng = json['longitude'][0]['value'];
    return Telemetry(
      latitude: double.parse(lat),
      longitude: double.parse(lng),
    );
  }
}