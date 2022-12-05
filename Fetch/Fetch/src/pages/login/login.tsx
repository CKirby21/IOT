import "./login.css";
import React, { useContext } from "react";
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
  IonCheckbox,
  NavContext,
} from "@ionic/react";
import Header from "../../components/header/header";
import { useForm } from 'react-hook-form';

// TODO use inputted email and password
const emailAndPassword = {"email": "example@gmail.com", "password": "password"};

const Login: React.FC = () => {

  const { handleSubmit } = useForm();
  const { navigate } = useContext(NavContext);
  const webSocket = new WebSocket('ws://localhost:3000');

  webSocket.onmessage = function onMessage(message) {
    if (message.data === 'email: true') {
      console.log('Logged In!');
      navigate('/map');
    }
  };
  const attemptToLogin = (data:any) => {
    const json = JSON.stringify(emailAndPassword);
    webSocket.send(json);
  }

  return (
    <IonPage>
      <IonContent>
        <IonHeader>
          <IonToolbar>
            <IonTitle>Login</IonTitle>
          </IonToolbar>
        </IonHeader>
        <Header />
        <form className="ion-padding" onSubmit={handleSubmit(attemptToLogin)}>
          <IonItem>
            <IonLabel position="floating">Email</IonLabel>
            <IonInput type="email" required />
          </IonItem>
          <IonItem>
            <IonLabel position="floating">Password</IonLabel>
            <IonInput type="password" required />
          </IonItem>
          <IonItem lines="none">
            <IonLabel>Remember me</IonLabel>
            <IonCheckbox defaultChecked={true} slot="start" />
          </IonItem>
          <IonButton className="ion-margin-top" type="submit" expand="block">
            Login
          </IonButton>
        </form>
      </IonContent>
    </IonPage>
  );
};

export default Login;
