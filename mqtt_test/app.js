/* 
#### Keys to Use in Sketch
verificationCert.key -- Key
verificationCert.pem -- Cert
*/

var awsIot = require('aws-iot-device-sdk');

var device = awsIot.device({
    //keyPath: "../jitp/verificationCert.key",
    //certPath: "../jitp/verificationCert.pem",
    keyPath: "certs/key.key",
    certPath: "certs/cert.pem",
    caPath: "certs/AmazonRootCA1.pem",
    clientId: "AQR_186413104",
    host: "a2q3xq7o4p30im-ats.iot.us-east-1.amazonaws.com"
}); 


function step1() {
    //Step 1 - Subscribe
    device.subscribe("$aws/certificates/create/json/rejected");
    device.subscribe("$aws/certificates/create/json/accepted"); 

    //Step 1 - Publish
    device.publish("$aws/certificates/create/json", JSON.stringify({ test_data: 1}));      
} 

function step2() {
    //Step 2 - Subscribe
    device.subscribe("$aws/provisioning-templates/Air_Quality/provision/json/accepted"); 
    device.subscribe("$aws/provisioning-templates/Air_Quality/provision/json/rejected");      

    var payLoad = {        
      "certificateOwnershipToken": "eyJ2ZXJzaW9uIjoiMjAxOTEwMjMiLCJjaXBoZXIiOiJBakZtcndnQzMvWitYNXp2ZW9JWUV1N0RyMXRlSm9vQkF5cE9ZTlR4V0lHSlJBKzJ4ZFVoVmZpOEZBRVlYdXBTdzloQmo1azZpa2tRNWtqSC92VTFxeHpzenRVWVZGUktDSmFrMTBFVWVKc0M5bmxBVXdZK0RXek1HWTV0bjRVWFBvMVJyeTlqS0VNL3ZwNStRM2VVM1JFcEE1cmpUTExLWEJkSHNxZU5URkVvd1hHeTBsQ1NFMk1vRGNlV2tub1VxM0krQWdCYnBrQWtWL0dDNGNnS1AySEtSYnQ3Y0VVODdwa1NFNk9IUmQvZ05nUEMvNUJwamVNQzdhRUpxQ3Y3VVJCdENuR2RIc0ZrT3FwUkk4Qkh6Z3pOeURzLzU1dzhlYThndFcwcWFCQVhoMDkzejljTTFxWT0ifQ==",
      "parameters": {
        "SerialNumber": "5000",
        "mac_addr": "FC:F5:C4:6D:C9:1D",
        "chip_id": "TD_1000",
        "token": "testtoken"
      }
    }    

    //Step 2 - Publish
    var send = JSON.stringify(payLoad);
    device.publish("$aws/provisioning-templates/Air_Quality/provision/json", send);     
}

//Connect Device
device.on('connect', () => {
    console.log("Connected");    

    //step1();
    //step2();
});

//Other Options
device.on('close', function (msg) {
    //console.log(msg);
    console.log('close');
  });
  device.on('reconnect', function () {
    console.log('reconnect');
  });
  device.on('offline', function () {
    console.log('offline');
  });
  device.on('error', function (error) {
    console.log('error', error);
  });

//Receive Message
device
  .on('message', function(topic, payload) {
    console.log('message', topic, payload.toString());
  });  

/* 
{
    "certificateId": "2afcc5be77be2dcf40f79fac4d03f86bb3a97e4f038e19603512da9937961db1",
    "certificatePem": "-----BEGIN CERTIFICATE-----\nMIIDWjCCAkKgAwIBAgIVAILj+hShtwTlaEX6vLNMDT7jgvpEMA0GCSqGSIb3DQEB\nCwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\nIEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjAxMzEwMDEw\nNDBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\ndGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC5WBeNDP84bBFZNabw\njP8n2nm5yxsSDXVbuygsR/FiGpIopuYgVl+cO8/Wkw9AAaXn2QAxLFVxxtizmjD7\nQbePCi+BGA1CzwJVa5bhkHWFFPGFTTapXM4accMo4Zce63ht0qxpIij2eDgakBiu\n3UxUMf3derYQQdG3uo8Fbrz87dOCTwyt4eYpr/H6fVnGlKJuGEqG4CUREA6+p1Jj\nj8NTxf6u465ariqL9cGTEtKG3hQ2oYdpZu4uJTAWNPdjOg+kt+Nun3ooeghO2+SD\nZDvJF30byElceALA+zkIQwaZsmhXOyQtikhnlDwp4aZ9UVB0ZxBExH04bdK7C7VY\nDAMlAgMBAAGjYDBeMB8GA1UdIwQYMBaAFHkwyKoM7X+b6J++4QK4YMNCAGJGMB0G\nA1UdDgQWBBTnl3gbPwwWooHbC8HJw3JL6cu2GzAMBgNVHRMBAf8EAjAAMA4GA1Ud\nDwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAgNNJIMgZO8DUbwwQ3mvv+dxh\npc6P3Vle/op6tWPw9h1jr+5Uk+KKHNsz4rf/RXjC/jtbpkFPA5v+JBAMsba01CrY\neSWzHQTY4m6qlkP7tUMZ8IFhXLWgraoNdOMOC/VGU6kGY18iJWDtx2aTjwwaxm9H\ny3yyYLv2aPeEZirw9R+U2HmsVU2wL4+UdKwQSzvgHJMeh/uOgi6efLo6C18GFmPp\nRMGNh6zxwh6il17Mt5aCSOy3DbcwSPs2DFPi4zcOX8kDsRW1/uFAXhns/zUV6AnN\ntZIlkNvWER8y/Iq5cW/sZOXt3b5t+2uD/VNK4wLs9CTuCHMmoU61/3JERo9+Mw==\n-----END CERTIFICATE-----\n",
    "privateKey": "-----BEGIN RSA PRIVATE KEY-----\nMIIEpQIBAAKCAQEAuVgXjQz/OGwRWTWm8Iz/J9p5ucsbEg11W7soLEfxYhqSKKbm\nIFZfnDvP1pMPQAGl59kAMSxVccbYs5ow+0G3jwovgRgNQs8CVWuW4ZB1hRTxhU02\nqVzOGnHDKOGXHut4bdKsaSIo9ng4GpAYrt1MVDH93Xq2EEHRt7qPBW68/O3Tgk8M\nreHmKa/x+n1ZxpSibhhKhuAlERAOvqdSY4/DU8X+ruOuWq4qi/XBkxLSht4UNqGH\naWbuLiUwFjT3YzoPpLfjbp96KHoITtvkg2Q7yRd9G8hJXHgCwPs5CEMGmbJoVzsk\nLYpIZ5Q8KeGmfVFQdGcQRMR9OG3Suwu1WAwDJQIDAQABAoIBAQCdce/PqlLEI0k5\ndxIxFhVKTZVJ9vcwBq7DFeGyfArZQ6kVT3H91mGx3j1YEeTKgpvkr7oyC28qD7jW\nGQ2y4siDgiuIhl39esyIs7SgOI8cVgY1LrpYdLXAURNwDvLJGU5cPLKpPJN/i/Bc\n4DuXj/XfL/hvxGYUuVTltlz4SHHSiOCGj/0sE5LhJbnRjARfW5iUHWWrSceWaodi\nrBz7J3+o8ehliCJBEmcQ3Djc5P7SQhX3E1LVlCdw2d5VRwlRfLni+2gZHdV2hK5b\nYww78ZEHCgQNt9SUAWeRhBMHezu4JCJnQEAljDHRVL6zjDjg311vjgEProS5mFHT\nX9j9WkShAoGBAPVfcMaHvX2dtuPHEr6PL+Jg9njUoTrPcpYO/cyihc6FZQXhjIzT\nl0t8skKn6cdz5R1Q/BQtLG1sAiaOdz2SIb/XgKz42yUPdPRs2nwQAa4xyGyQg1JO\npc+hUcM+7QF/7z9oiah2hwN8cp6lKjJIx0bPklB/2RwT/Oi2tuVnhPgzAoGBAMFf\nFgaZbc2UIwQDo+SgpBj3b+jVAwywHPwjfPA3/LM2jVSpXD7N9M5X5xRErDVPFAB0\n0KERTYkWAUVGrVy07NnT0B8nMxqyaUyudenx+pIKG1P5wmXWpmgoLgTobc5rd3pA\nrdgYzyUlXx9yybd3DH3lOqVd6wcyp4blYsVubx9HAoGBAKNSgZhDpadFIQT/7sQU\nPsUp0SSgMykYB6dGFzzsOLA8Lf/6HzllZWEDgQvJBFyUEb6vtvofD2FN8SiEz22W\nJQyyfp2jjwpd/XTsN4k4hkXSMRvCHQbd/pPzRp709YB4DWFhTa9qJXf2j7x1J18M\n/kdtxm/6iSKdU0v6wkZz3xftAoGBAK1ttHlyvhI1HDqLcUCp+sMRmOL/uN52TWiu\nA24HKLMq6BLYWo1RdpBiMjPxa2ncZJoMKtlqhVm/zJ/vRajx88NhvzVRmPoGrX5C\nsiqCBjNYsS3dwUZI/7JG5w8rzt/lbqDMxZ4ckhxEqk2wS84qDaAUKUcLNrGb8rh/\nnthntW4dAoGAeggb0zx5W1aIsX6nVC7lVIqONWH/o/ozV5oRJX4MAc+xMk5u4A1S\n9cc/I8zJgSZXiUU5EdVVa4URJDEbI8vvqF6e4ktODszDih/kz5EJARWf08PyInL4\nNu10H5is56+pKwregnDNheKKYdHRxW8qkLt8UIxffFdKhB3JfMWgzuk=\n-----END RSA PRIVATE KEY-----\n",
    "certificateOwnershipToken": "eyJ2ZXJzaW9uIjoiMjAxOTEwMjMiLCJjaXBoZXIiOiJBakZtcndnQzMvWitYNXp2ZW9JWUV1N0RyMXRlSm9vQkF5cE9ZTlR4V0lHSlJBKzJ4ZFVoVmZpOEZBRVlYdXBTdzloQmo1azZpa2tRNWtqSC92VTFxeHpzenRVWVZGUktDSmFrMTBFVWVKc0M5bmxBVXdZK0RXek1HWTV0bjRVWFBvMVJyeTlqS0VNL3ZwNStRM2VVM1JFcEE1cmpUTExLWEJkSHNxZU5URkVvd1hHeTBsQ1NFMk1vRGNlV2tub1VxM0krQWdCYnBrQWtWL0dDNGNnS1AySEtSYnQ3Y0VVODdwa1NFNk9IUmQvZ05nUEMvNUJwamVNQzdhRUpxQ3Y3VVJCdENuR2RIc0ZrT3FwUkk4Qkh6Z3pOeURzLzU1dzhlYThndFcwcWFCQVhoMDkzejljTTFxWT0ifQ=="
}
*/

/* 
{
    "deviceConfiguration": {
        "success": "true"
    },
    "thingName": "AQR_5000"
}
*/

/* 
LAMBDA RESPONSE
2022-01-31T00:17:21.620Z	347fbd12-b13e-4438-a992-f7026e859f85	INFO	{
  chip_id: 'TD_1000',
  SerialNumber: '5000',
  mac_addr: 'FC:F5:C4:6D:C9:1D',
  token: 'testtoken'
}
*/