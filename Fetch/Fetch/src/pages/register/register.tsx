import "./register.css";
import React from "react";
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
} from "@ionic/react";
import Header from "../../components/header/header";

const Register: React.FC = () => {
  return (
    <IonPage>
      <IonContent>
        <IonHeader>
          <IonToolbar>
            <IonTitle>Register</IonTitle>
          </IonToolbar>
        </IonHeader>
        <Header />
        <form className="ion-padding">
          <IonItem>
            <IonLabel position="floating">Username</IonLabel>
            <IonInput required />
          </IonItem>
          <IonItem>
            <IonLabel position="floating">Email</IonLabel>
            <IonInput type="email" required />
          </IonItem>
          <IonItem>
            <IonLabel position="floating">Password</IonLabel>
            <IonInput type="password" required />
          </IonItem>
          <IonButton className="ion-margin-top" type="submit" expand="block">
            Sign Up
          </IonButton>
        </form>
      </IonContent>
    </IonPage>
  );
};

export default Register;
