import './register.css';
import React from 'react';
import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import Header from "../../components/header/header";

const Register: React.FC = () => {
  return(
    <IonPage>
      <IonContent>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Register</IonTitle>
        </IonToolbar>
      </IonHeader>
        <Header />
      </IonContent>
    </IonPage>
  );
};

export default Register;