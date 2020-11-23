class AuthResponse {
  final String token;
  final String refreshToken;

  AuthResponse({this.token, this.refreshToken});

  factory AuthResponse.fromJson(Map<String, dynamic> json) {
    return AuthResponse(
      token: json['token'],
      refreshToken: json['refreshToken'],
    );
  }
}