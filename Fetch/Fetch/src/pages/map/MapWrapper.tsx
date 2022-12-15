import {
  IonContent,
  IonHeader,
  IonItem,
  IonLabel,
  IonPage,
  IonTitle,
  IonToolbar,
  IonInput,
  IonButton,
  IonNote,
  IonGrid,
  IonRow,
  IonCol,
  IonText,
} from "@ionic/react";
import React, { useState } from "react";
import Header from "../../components/header/header";
import {
  CircleF,
  GoogleMap,
  LoadScript,
  MarkerF,
  OverlayViewF,
} from "@react-google-maps/api";
import emailjs from "@emailjs/browser";

const mapContainerStyle = {
  width: "1100px",
  height: "600px",
};

const circleOptions = {
  strokeColor: "#FF0000",
  strokeOpacity: 0.8,
  strokeWeight: 2,
  fillColor: "#FF0000",
  fillOpacity: 0.35,
  clickable: false,
  draggable: false,
  editable: false,
  visible: true,
};

const overlayStyle = {
  background: "white",
  border: "1px solid #ccc",
  padding: 5,
};

const MapWrapper: React.FC<{}> = () => {
  const minRadius = 20;
  const maxRadius = 1000;
  const [isValidRadius, setIsValidRadius] = useState<boolean>();
  const [radius, setRadius] = useState(minRadius);
  const [savedRadius, setSavedRadius] = useState<number>(minRadius);
  const [center, setCenter] = useState({lat: 0, lng: 0});
  const [savedCenter, setSavedCenter] = useState({lat: 40.8206975, lng: -96.7055880});
  const [deviceCoordinates, setDeviceCoordinates] = useState({lat: 40.8206975, lng: -96.7055880});
  const [deviceUpdateTime, setDeviceUpdateTime] = useState("");

  /* Functions for Radius */
  const validateRadius = (ev: Event) => {
    const input = (ev.target as HTMLInputElement).value;
    setIsValidRadius(undefined);
    if (input === "") return;

    const inputRadius = parseInt(input);
    setRadius(inputRadius);
    if (minRadius <= inputRadius && inputRadius <= maxRadius) {
      setIsValidRadius(true);
    } else {
      setIsValidRadius(false);
    }
  };

  const saveRadius = () => {
    if (isValidRadius) setSavedRadius(radius);
  };

  /* Functions for Center */
  function getCenterSuccess(position: { coords: any }) {
    setCenter({
      lat: position.coords.latitude,
      lng: position.coords.longitude,
    });
  }

  function getCenterError(err: { code: any; message: any }) {
    console.warn(`ERROR(${err.code}): ${err.message}`);
  }

  const getCenter = () => {
    const options = {
      enableHighAccuracy: true,
      timeout: 5000,
      maximumAge: 0,
    };
    navigator.geolocation.getCurrentPosition(
      getCenterSuccess,
      getCenterError,
      options
    );
  };

  const saveCenter = () => {
    if (center.lat !== 0 && center.lng !== 0) setSavedCenter(center);
  };

  const formatCoordinates = (coords: any) => {
    if (coords.lat === 0 && coords.lng === 0) return "";
    return coords.lat.toFixed(3) + ", " + coords.lng.toFixed(3);
  };
  
  /* Determine if dog is outside fence */
  function haversine_distance(mk1 : {lat: number, lng: number }, mk2 : {lat: number, lng: number }) {
    const R = 6367449 // Radius of the Earth in meters
    const rlat1 = mk1.lat * (Math.PI/180); // Convert degrees to radians
    const rlat2 = mk2.lat * (Math.PI/180); // Convert degrees to radians
    const difflat = rlat2-rlat1; // Radian difference (latitudes)
    const difflon = (mk2.lng-mk1.lng) * (Math.PI/180); // Radian difference (longitudes)

    const d = 2 * R * Math.asin(Math.sqrt(Math.sin(difflat/2)*Math.sin(difflat/2)+Math.cos(rlat1)*Math.cos(rlat2)*Math.sin(difflon/2)*Math.sin(difflon/2)));
    return d;
  }

  const checkDogLocation = () => {
    const d = haversine_distance(savedCenter, deviceCoordinates);
    console.log(d.toFixed(2));
    if (d > savedRadius) 
      console.log('email');
  };

  /* Functions Device Coordinates from Backend */
  const webSocket = new WebSocket("ws://localhost:3000");
  webSocket.onmessage = function onMessage(message) {
    try {
      const messageData = JSON.parse(message.data);
      console.log(messageData);

      // time, latitude and longitude are required
      if (
        !messageData.MessageDate ||
        !messageData.IotData.latitude ||
        !messageData.IotData.longitude
      ) {
        return;
      }

      // Update the device data
      setDeviceCoordinates({
        lat: messageData.IotData.latitude,
        lng: messageData.IotData.longitude,
      });
      setDeviceUpdateTime(messageData.MessageDate);
      checkDogLocation();
      
    } catch (err) {
      console.error(err);
    }
  };

  // Send email
  const templateParams = {
    to_name: "Michael Jordan",
    message: "Your dog has escaped your defined geofence.",
  };

  function sendDogLocation() {
    emailjs
      .send(
        "service_p9scn9d",
        "template_olnfkfq",
        templateParams,
        "hCAMVBKsuZNcevWIn"
      )
      .then(
        (response) => {
          console.log("Success!", response.status, response.text);
        },
        (err) => {
          console.log("Failed...", err);
        }
      );
  }

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Map</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>
        <Header />
        <IonGrid>
          <IonRow>
            <LoadScript googleMapsApiKey="AIzaSyBYrl_wYHZ1QBg_0CNqu7XOKbzi9sIjJ2E">
              <GoogleMap
                mapContainerStyle={mapContainerStyle}
                center={savedCenter}
                zoom={17}
              >
                {/* Saved Fence Coordinates */}
                <MarkerF position={savedCenter} />
                <CircleF
                  center={savedCenter}
                  radius={savedRadius}
                  options={circleOptions}
                />

                {/* Dog's Coordinates */}
                <OverlayViewF
                  position={deviceCoordinates}
                  mapPaneName="floatPane"
                >
                  <div style={overlayStyle}>
                    <p>
                      {"Last Updated: " +
                        deviceUpdateTime.replace(/[A-Z]/gm, " ")}
                    </p>
                  </div>
                </OverlayViewF>
                <MarkerF label="Dog" position={deviceCoordinates} />
              </GoogleMap>
            </LoadScript>

            <IonGrid fixed={true}>
              <IonRow>
                <IonCol>
                  <IonItem>
                    <IonText>
                      <h1>Fence Settings</h1>
                    </IonText>
                  </IonItem>

                  <IonItem />

                  <IonItem>
                    <IonLabel>Saved Center:</IonLabel>
                    <IonInput
                      value={formatCoordinates(savedCenter)}
                      disabled={true}
                    ></IonInput>
                  </IonItem>

                  <IonItem>
                    <IonLabel>
                      {"New Center: " + formatCoordinates(center)}
                    </IonLabel>
                    <IonButton onClick={() => getCenter()}>Fetch</IonButton>
                  </IonItem>

                  <IonItem>
                    <IonButton onClick={() => saveCenter()}>Save</IonButton>
                  </IonItem>

                  <IonItem />

                  <IonItem>
                    <IonLabel>Saved Radius:</IonLabel>
                    <IonInput
                      value={savedRadius + " meters"}
                      disabled={true}
                    ></IonInput>
                  </IonItem>

                  <IonItem
                    className={`${isValidRadius && "ion-valid"} ${
                      isValidRadius === false && "ion-invalid"
                    }`}
                  >
                    <IonLabel>New Radius:</IonLabel>
                    <IonInput
                      type="number"
                      defaultValue={minRadius}
                      onIonInput={(event) => validateRadius(event)}
                    ></IonInput>
                    <IonNote slot="error">
                      Radius must be a number between {minRadius} and{" "}
                      {maxRadius} meters
                    </IonNote>
                  </IonItem>

                  <IonItem>
                    <IonButton onClick={() => saveRadius()}>Save</IonButton>
                  </IonItem>

                  <IonItem>
                    <IonButton onClick={() => sendDogLocation()}>
                      Send Dog Location
                    </IonButton>
                  </IonItem>
                </IonCol>
              </IonRow>
            </IonGrid>
          </IonRow>
        </IonGrid>
      </IonContent>
    </IonPage>
  );
};

export default MapWrapper;
